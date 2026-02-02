# Standard Phoebus script utilities
from org.csstudio.display.builder.runtime.script import ScriptUtil, PVUtil

# For handling VTypes (if you need to check timestamps or alarms)
from org.phoebus.archive.vtype import VTypeHelper 

# Standard Python/Java imports
from time import sleep
from java.lang import Exception as JavaException # Use generic Exception for safety

pvTimeoutMs = 1000  # used to ensure PVs are synchronous.
pvPollMs = 1000 / 50


# Needed because IPV.setValue() doesn't take VType. Yuck.
def cast(value):
    if VTypeHelper.getBasicDataType(value) is BasicDataType.STRING:
        return VTypeHelper.getString(value)
    elif VTypeHelper.getBasicDataType(value) is BasicDataType.STRING_ARRAY:
        return VTypeHelper.getString(value)
    elif VTypeHelper.getSize(value) > 1:
        return VTypeHelper.getDoubleArray(value)
    else:
        return VTypeHelper.getDouble(value)


# PV value get, ensures PV connection, best-effort type handling.
def pvGet(sourcePV):
    isConnected = False
    for i in range(pvTimeoutMs / pvPollMs):
        sleep(pvPollMs / 1000.)
        if sourcePV.isConnected():
            value = sourcePV.getValue()
            if value is not None:
                isConnected = True
                break
    if not isConnected:
        raise RuntimeError(
            'Timeout waiting for PV to connect: ' + sourcePV.name)
    return cast(value)


# PV value set, ensures PV connection.
def pvSet(destPV, value):
    isConnected = False
    for i in range(pvTimeoutMs / pvPollMs):
        sleep(pvPollMs / 1000.)
        if destPV.isWriteAllowed():
            isConnected = True
            break
    if not isConnected:
        raise RuntimeError(
            'Timeout waiting for PV to connect: ' + destPV.name)
    try:
        destPV.setValue(value, pvTimeoutMs)
    except RuntimeException:
        raise RuntimeError('Error writing PV {} with VAL {}'
                           .format(destPV.name, str(value)))


# PV value copy, ensures PV connection, best-effort type handling.
def pvCopy(sourcePV, destPV):
    pvSet(destPV, pvGet(sourcePV))


# Creates a new IPV object, like PVUtil.create(), except it doesn't
# tie it to a widget, so make SURE it is stopped after use.
def pvCreate(pvName):
    return pvName

# Wraps pvGet() with connect() and stop().
def pvConnectAndGet(pvName):
    pv = pvCreate(pvName)
    val = pvGet(pv)
    pv.stop()
    return val

# pvConnectAndGet() for list of PVs.
def pvConnectAndGetList(pvNames):
    return pvNames

# Wraps pvSet() with connect() and stop().
def pvConnectAndSet(pvName, value):
    pv = pvCreate(pvName)
    pvSet(pv, value)
    pvGet(pv)
    pv.stop()

# pvConnectAndSet() for list of PVs.
def pvConnectAndSetList(pvNames, values):
    timweout = 1
    for name, value in zip(pvNames, values):
        try:
            # This replaces the need for start(), write(), and stop()
            PVUtil.writePV(name, value, timeout)
            print("Successfully set " + name + " to " + str(value))
        except Exception as e:
            print("Failed to set PV " + name + ": " + str(e))    pvs = [pvCreate(n) for n in pvNames]
"""            
    [pvSet(pv, value) for pv, value in zip(pvs, values)]
    [pvGet(pv) for pv in pvs]
    [pv.stop() for pv in pvs]
"""
