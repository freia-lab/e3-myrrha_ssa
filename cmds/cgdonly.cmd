require myrrha_ssa
require ecat2

epicsEnvSet(IOC_NUM, "1")
epicsEnvSet(CAB_NUM, "190")
epicsEnvSet(IOC_CAB_NUM, "100")
epicsEnvSet(DISCIPLINE, "RFS")

epicsEnvSet(CAB_SSA, "CAB-$(CAB_NUM)")
epicsEnvSet(CAB_IOC, "CAB-$(IOC_CAB_NUM)")

epicsEnvSet(PREFIX_ECAT, "$(CAB_IOC):$(DISCIPLINE)-IOC")
iocshLoad("$(ecat2_DIR)ecat2.iocsh", "ECAT_FREQ=100")
#ecat2sts d0.r0 d0.r249
#ecat2sts d1.r0 d1.r249

dbLoadRecords("$(myrrha_ssa_DB)ecat.db", "PREFIX=$(PREFIX_ECAT),IOC_NUM=$(IOC_NUM)")

epicsEnvSet(PREFIX_CGD, "$(CAB_SSA):$(DISCIPLINE)-CGD")
epicsEnvSet(NUM_PSU, "5")
epicsEnvSet(NUM_SSM, "8")
epicsEnvSet(NUM_AMP, "2")
iocshLoad("$(myrrha_ssa_DIR)cab.iocsh", "SLAVE_ID=0")

