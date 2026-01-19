// A port driver for managing SSM buffered data download.
// It read and writes the configuration mask, and downloads the buffer.

#include <asynPortDriver.h>
#include <asynOctetSyncIO.h>
#include <asynCommonSyncIO.h>
#include <alarm.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsExit.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <chrono>

extern "C" {
#include <asynOctet.h>
}

class BfrDwnldDriver: public asynPortDriver {
public:

    BfrDwnldDriver(char const *portName, char const *ssmPortName);
    ~BfrDwnldDriver();
    asynStatus readUInt32Digital(asynUser *user, epicsUInt32 *value,
                                 epicsUInt32 mask) override;
    asynStatus writeUInt32Digital(asynUser *user, epicsUInt32 value,
                                  epicsUInt32 mask) override;

private:

    // There is a hard requirement on the download time, so it doesn't need to
    // be configurable.
    static constexpr double totalDownloadTimeout = 20;
    static constexpr double writeReadTimeout = 0.5;

    // Size of trigger mask data.
    static constexpr size_t numMaskRecords = 9;
    static constexpr size_t numBytesInMask = 2 * numMaskRecords;

    // Size of buffered data.
    static constexpr size_t numBytesInFrame = 1400;
    static constexpr size_t numFrames = 1124;

    struct Commands {
        static constexpr char const *startDump = "\xaa";
        static constexpr char const *readMask = "\x11";
        static constexpr char const *writeMask = "\x01";
        static constexpr char const *readTrigger = "\x0e";
        static constexpr char const *enableTrigger = "\xee";
        static constexpr char const *forceTrigger = "\xfe";
    };

    struct Responses {
        static constexpr char const error = '\x00';
        static constexpr char const startDump = '\x1a';
        static constexpr char const startFrame = '\xdd';
        static constexpr char const startMask = '\x01';
        static constexpr char const maskAccepted = '\x01';
        static constexpr char const triggerUnarmed = '\x1e';
        static constexpr char const triggerArmed = '\x0e';
        static constexpr char const triggerNowArmed = '\x1e';
        static constexpr char const triggerAlreadyArmed = '\x00';
        static constexpr char const triggerForced = '\x0e';
    };

    asynUser *octetUser = nullptr;
    asynUser *commonUser = nullptr;
    bool isOK = false;
    std::vector<char> bufferData;

    int firstMaskParam = -1;
    int lastMaskParam = -1;
    int &firstParam = firstMaskParam;
    int lastParam = -1;

    int bfrDwnldIndParam = -1;
    int bfrDwnldTrigParam = -1;
    int bfrDwnldTrigStatParam = -1;
    int bfrDwnldTrigEnableParam = -1;
    int bfrDwnldTrigForceParam = -1;
    int maskReadTrigParam = -1;
    int maskWriteTrigParam = -1;

    bool isMaskParam(int param);
    bool isOurParam(int param);
    asynStatus readMask();
    asynStatus writeMask();
    asynStatus readBuffer();
    asynStatus manipulateTrigger(int reason);
};


// A simple class that stores a function and executes it when it goes out of
// scope. Useful for cleanup code.
template <typename Function>
class ScopeGuard {
    Function func;
public:
    ScopeGuard(Function &&func) : func(std::forward<Function>(func)) {}
    ~ScopeGuard() { func(); }
};

template <typename Function>
ScopeGuard<Function> makeScopeGuard(Function &&func){
    return {std::forward<Function>(func)};
};


BfrDwnldDriver::BfrDwnldDriver(const char *portName, const char *ssmPortName) :
    asynPortDriver(portName, 1,
                   asynCommonMask | asynDrvUserMask | asynInt8ArrayMask
                   | asynUInt32DigitalMask,
                   asynUInt32DigitalMask | asynInt8ArrayMask,
                   ASYN_CANBLOCK, 1, 0, 0) {
    auto ostatus = pasynOctetSyncIO->connect(ssmPortName, 0, &octetUser,
                                             nullptr);
    auto sstatus = pasynCommonSyncIO->connect(ssmPortName, 0, &commonUser,
                                              nullptr);
    if (sstatus || ostatus) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "cannot connect to SSM port %s\n", ssmPortName);
        return;
    }

    createParam("SSMStat", asynParamUInt32Digital, &firstMaskParam);
    createParam("PallStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("RstStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("PwrStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("VoltLimStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("CurrLimStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("TWarnStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("TLimStat", asynParamUInt32Digital, &lastMaskParam);
    createParam("RefStat", asynParamUInt32Digital, &lastMaskParam);

    createParam("MaskReadTrig", asynParamUInt32Digital, &maskReadTrigParam);
    createParam("MaskWriteTrig", asynParamUInt32Digital, &maskWriteTrigParam);
    createParam("BfrDwnldTrig", asynParamUInt32Digital, &bfrDwnldTrigParam);
    createParam("BfrDwnldTrigStat", asynParamUInt32Digital,
                &bfrDwnldTrigStatParam);
    createParam("BfrDwnldTrigEnable", asynParamUInt32Digital,
                &bfrDwnldTrigEnableParam);
    createParam("BfrDwnldTrigForce", asynParamUInt32Digital,
                &bfrDwnldTrigForceParam);
    createParam("BfrDwnldInd", asynParamUInt32Digital, &bfrDwnldIndParam);
    createParam("BfrData", asynParamInt8Array, &lastParam);

    setUIntDigitalParam(bfrDwnldIndParam, 0, 0xffff);

    // Ensure that mask param indices can be used as array indices.
    if (firstMaskParam != 0 || lastMaskParam != (numMaskRecords - 1)) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: internal error, param indices don't match assumption.\n",
                  portName);
        return;
    }

    isOK = true;
    bufferData.resize(numBytesInFrame * numFrames);

    // Integer param functions will just return the last read value, so
    // ensure that there is a value there.
    readMask();
}

BfrDwnldDriver::~BfrDwnldDriver() {
    lock();
    if (octetUser) {
        pasynOctetSyncIO->disconnect(octetUser);
    }
    if (commonUser) {
        pasynCommonSyncIO->disconnect(commonUser);
    }
    unlock();
}

bool BfrDwnldDriver::isOurParam(int param) {
    return param <= lastParam && param >= firstParam;
}

bool BfrDwnldDriver::isMaskParam(int param) {
    return param <= lastMaskParam && param >= firstMaskParam;
}

asynStatus BfrDwnldDriver::readMask() {
    char incoming[numBytesInMask + 1];  // Includes sync byte
    size_t nRead;
    size_t nWritten;
    int eomReason;
    asynStatus status =
        pasynOctetSyncIO->writeRead(octetUser,
                                    Commands::readMask, 1,
                                    incoming, sizeof(incoming),
                                    writeReadTimeout, &nWritten, &nRead,
                                    &eomReason);

    // Ensure that alarms are set if we return early.
    auto setAlarms = makeScopeGuard([this, &status]() {
        for (size_t i = 0; i < numMaskRecords; ++i) {
            if (status != asynSuccess) {
                setParamAlarmSeverity(i, INVALID_ALARM);
                if (status == asynTimeout) {
                    setParamAlarmStatus(i, TIMEOUT_ALARM);
                } else {
                    setParamAlarmStatus(i, COMM_ALARM);
                }
            } else {
                setParamAlarmSeverity(i, NO_ALARM);
            }
        }
    });

    if (status != asynSuccess
        || eomReason != ASYN_EOM_CNT
        || nWritten != 1
        || nRead != sizeof(incoming)) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: mask read error: "
                  "status %d eom %d nWritten %lu nRead %lu\n.",
                  portName, status, eomReason, nWritten, nRead);
        if (!status) {
            status = asynError;
        }
        return status;
    }

    if (incoming[0] != Responses::startMask) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: mask read error, response does not "
                  "begin with the sync byte.\n",
                  portName);
        if (!status) {
            status = asynError;
        }
    }

    if (status) {
        return status;
    }

    for (size_t f = 0; f < numMaskRecords; ++f) {
        uint16_t val;
        auto source = incoming + 2 * f + 1;
        auto dest = reinterpret_cast<char*>(&val);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        dest[0] = source[1];
        dest[1] = source[0];
#else
        dest[0] = source[0];
        dest[1] = source[1];
#endif
        setUIntDigitalParam(f, val, 0xffff);
    }

    callParamCallbacks();
    return status;
}

asynStatus BfrDwnldDriver::writeMask() {
    char outgoing[numBytesInMask + 1];  // Includes sync byte
    char incoming[1];

    outgoing[0] = Commands::writeMask[0];
    for (size_t f = 0; f < numMaskRecords; ++f) {
        uint32_t rval;
        getUIntDigitalParam(f, &rval, 0xffff);
        uint16_t val = rval;
        auto source = reinterpret_cast<const char*>(&val);
        auto dest = outgoing + 2 * f + 1;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        dest[0] = source[1];
        dest[1] = source[0];
#else
        dest[0] = source[0];
        dest[1] = source[1];
#endif
    }

    size_t nRead;
    size_t nWritten;
    int eomReason;
    asynStatus status =
        pasynOctetSyncIO->writeRead(octetUser,
                                    outgoing, sizeof(outgoing), incoming, 1,
                                    writeReadTimeout, &nWritten, &nRead,
                                    &eomReason);
    if (status != asynSuccess
        || eomReason != ASYN_EOM_CNT
        || nWritten != sizeof(outgoing)
        || nRead != sizeof(incoming)) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: mask write error: "
                  "status %d eom %d nWritten %lu nRead %lu.\n",
                  portName, status, eomReason, nWritten, nRead);
        if (!status) {
            status = asynError;
        }
        return status;
    }

    if (incoming[0] != Responses::maskAccepted) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: mask write error, response does not "
                  "match the sync byte.\n",
                  portName);
        if (!status) {
            status = asynError;
        }
    }

    return status;
}

asynStatus BfrDwnldDriver::readBuffer() {
    auto startTime = std::chrono::steady_clock::now();
    auto elapsedTime = [startTime]() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - startTime;
        return std::chrono::duration_cast<
            std::chrono::duration<double>>(elapsed).count();
    };

    size_t nRead = 0;
    size_t nWritten = 0;
    int eomReason = -1;
    asynStatus status = asynSuccess;

    auto unlockAndReportError = makeScopeGuard([&]() {
        pasynManager->unlockPort(octetUser);
        if (status) {
            if (status == asynTimeout && elapsedTime() > totalDownloadTimeout) {
                asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                          "%s: buffer download error, time required "
                          "exceeds %.1f seconds.\n",
                          portName, totalDownloadTimeout);
            } else {
                asynPrint(octetUser, ASYN_TRACE_ERROR,
                        "%s: buffer download error: "
                        "status %d eom %d nWritten %lu nRead %lu.\n",
                        portName, status, eomReason, nWritten, nRead);
            }
        }
    });

    status = pasynManager->lockPort(octetUser);
    if (status) {
        return status;
    }

    auto interface = pasynManager->findInterface(octetUser,
                                                 asynOctetType, 0);
    if (!interface) {
        status = asynError;
        return status;
    }

    void* drvPvt = interface->drvPvt;
    auto pasynOctet = static_cast<asynOctet*>(interface->pinterface);
    char frameBuffer[numBytesInFrame + 1];  // Includes sync byte
    octetUser->timeout = writeReadTimeout;

    status = pasynOctet->flush(drvPvt, octetUser);
    if (status) {
        return status;
    }

    status = pasynOctet->write(drvPvt, octetUser, Commands::startDump,
                               1, &nWritten);
    if (status || nWritten != 1) {
        if (!status) {
            status = asynError;
        }
        return status;
    }

    nWritten = 0;
    status = pasynOctet->read(drvPvt, octetUser,
                              frameBuffer, 1, &nRead,
                              &eomReason);
    if (status
        || nRead != 1
        || eomReason != ASYN_EOM_CNT) {
        if (!status) {
            status = asynError;
        }
        return status;
    }

    if (frameBuffer[0] != Responses::startDump) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: buffer download error, SSM did not accept "
                  "command: 0x%x.\n",
                  portName, (unsigned int)frameBuffer[0]);
        status = asynError;
        return status;
    }

    for (size_t frameNo = 0; frameNo < numFrames; ++frameNo) {
        size_t totalRead = 0;
        do {
            status = pasynOctet->read(drvPvt, octetUser,
                                      frameBuffer + totalRead,
                                      sizeof(frameBuffer) - totalRead,
                                      &nRead,
                                      &eomReason);
            totalRead += nRead;
            if (elapsedTime() > totalDownloadTimeout) {
                status = asynTimeout;
                break;
            }
        } while ((status == asynSuccess || status == asynTimeout)
                 && totalRead < sizeof(frameBuffer));

        if (status) {
            return status;
        }

        if (frameBuffer[0] != Responses::startFrame) {
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                    "%s: buffer download error, response does not "
                    "start with the sync byte.\n",
                    portName);
            status = asynError;
            return status;
        }

        memcpy(bufferData.data() + frameNo * numBytesInFrame,
               frameBuffer + 1, numBytesInFrame);
    }

    if (elapsedTime() > totalDownloadTimeout) {
        status = asynTimeout;
    }

    int paramIndex;
    findParam("BfrData", &paramIndex);
    doCallbacksInt8Array(bufferData.data(), bufferData.size(), paramIndex, 0);
    return status;
}

asynStatus BfrDwnldDriver::manipulateTrigger(int reason) {
    char const *command;
    char const *commandStr;
    if (reason == bfrDwnldTrigStatParam) {
        command = Commands::readTrigger;
        commandStr = "readTrigger";
    } else if (reason == bfrDwnldTrigEnableParam) {
        command = Commands::enableTrigger;
        commandStr = "enableTrigger";
    } else if (reason == bfrDwnldTrigForceParam) {
        command = Commands::forceTrigger;
        commandStr = "forceTrigger";
    } else {
        fprintf(stderr, "Logic error in %s at %d\n", __FILE__, __LINE__);
        abort();
    }

    char incoming[1];
    size_t nRead = 0;
    size_t nWritten = 0;
    int eomReason = -1;
    asynStatus status =
        pasynOctetSyncIO->writeRead(octetUser,
                                    command, 1, incoming, 1,
                                    writeReadTimeout, &nWritten, &nRead,
                                    &eomReason);

    if (status != asynSuccess
        || eomReason != ASYN_EOM_CNT
        || nWritten != 1
        || nRead != 1) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: command %s error: "
                  "status %d eom %d nWritten %lu nRead %lu.\n",
                  portName, commandStr, status, eomReason, nWritten, nRead);
        if (!status) {
            status = asynError;
        }
        return status;
    }

    if (reason == bfrDwnldTrigStatParam) {
        if (incoming[0] == Responses::triggerUnarmed) {
            setUIntDigitalParam(reason, 0, 0xffff);
            callParamCallbacks();
        } else if (incoming[0] == Responses::triggerArmed) {
            setUIntDigitalParam(reason, 1, 0xffff);
            callParamCallbacks();
        } else {
            status = asynError;
        }
    } else if (reason == bfrDwnldTrigEnableParam) {
        if (incoming[0] != Responses::triggerNowArmed
            && incoming[0] != Responses::triggerAlreadyArmed) {
            status = asynError;
        }
    } else if (reason == bfrDwnldTrigForceParam) {
        if (incoming[0] != Responses::triggerForced) {
            status = asynError;
        }
    }

    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: command %s error: response was 0x%02x\n",
                  portName, commandStr, int(incoming[0]));
    }

    return status;
}

asynStatus BfrDwnldDriver::readUInt32Digital(asynUser *user, epicsUInt32 *value,
                                             epicsUInt32 mask) {
    if (!isOK) {
        return asynError;
    }

    asynStatus status;
    if (user->reason == bfrDwnldTrigStatParam) {
        status = manipulateTrigger(user->reason);
        if (!status) {
            getUIntDigitalParam(user->reason, value, mask);
        }
    } else {
        return asynPortDriver::readUInt32Digital(user, value, mask);
    }

    // Requirements specify that, should things go pear shaped, the IOC
    // must reconnect to the SSM.
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: reconnecting to clear network buffers.\n",
                  portName);
        pasynOctetSyncIO->flush(octetUser);
        pasynCommonSyncIO->disconnectDevice(commonUser);
        pasynCommonSyncIO->connectDevice(commonUser);
    }

    return status;
}

asynStatus BfrDwnldDriver::writeUInt32Digital(asynUser *user, epicsUInt32 value,
                                              epicsUInt32 mask) {
    if (!isOK) {
        return asynError;
    }

    asynStatus status;
    if (user->reason == maskReadTrigParam) {
        status = readMask();
    } else if (user->reason == maskWriteTrigParam) {
        status = writeMask();
    } else if (user->reason == bfrDwnldTrigEnableParam
               || user->reason == bfrDwnldTrigForceParam) {
        status = manipulateTrigger(user->reason);
    } else if (user->reason == bfrDwnldTrigParam) {
        setUIntDigitalParam(bfrDwnldIndParam, 1, 0xffff);
        setParamAlarmSeverity(bfrDwnldIndParam, NO_ALARM);
        setParamAlarmStatus(bfrDwnldIndParam, NO_ALARM);
        callParamCallbacks();

        status = readBuffer();

        setUIntDigitalParam(bfrDwnldIndParam, 0, 0xffff);
        if (status != asynSuccess) {
            setParamAlarmSeverity(bfrDwnldIndParam, INVALID_ALARM);
            if (status == asynTimeout) {
                setParamAlarmStatus(bfrDwnldIndParam, TIMEOUT_ALARM);
            } else {
                setParamAlarmStatus(bfrDwnldIndParam, COMM_ALARM);
            }
        }
        callParamCallbacks();
    } else if (user->reason == bfrDwnldTrigStatParam) {
        asynPrint(octetUser, ASYN_TRACE_ERROR,
                  "%s: writing to a read-only property.",
                  portName);
        return asynError;
    } else if (isOurParam(user->reason)) {
        // Only cache the trigger mask, but don't call callbacks.
        // They will be called when the values are actually written.
        return setUIntDigitalParam(user->reason, value, 0xffff);
    } else {
        return asynPortDriver::writeUInt32Digital(user, value, mask);
    }

    // Requirements specify that, should things go pear shaped, the IOC
    // must reconnect to the SSM.
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s: reconnecting to clear network buffers.\n",
                  portName);
        pasynOctetSyncIO->flush(octetUser);
        pasynCommonSyncIO->disconnectDevice(commonUser);
        pasynCommonSyncIO->connectDevice(commonUser);
    }

    return status;
}


extern "C" {

static int const numArgs = 2;
static iocshArg const arg1 = {"asyn port name for this driver", iocshArgString};
static iocshArg const arg2 = {"asyn port name of the SSM", iocshArgString};
static iocshArg const * const args[numArgs] = { &arg1, &arg2 };
static iocshFuncDef cfgFunc = {"bufferDownloadConfigure", numArgs, args};

static void callFunc(iocshArgBuf const *args) {
    auto driver = new BfrDwnldDriver(args[0].sval, args[1].sval);
    auto destroyMe = [](void *p) {
        auto ptr = static_cast<BfrDwnldDriver*>(p);
        delete ptr;
    };
    epicsAtExit(destroyMe, driver);
}

static void BfrDwnldRegister() {
    iocshRegister(&cfgFunc, callFunc);

}

epicsExportRegistrar(BfrDwnldRegister);

}
