require myrrha_ssa,master
require ecat2,3.0.5

epicsEnvSet("STREAM_PROTOCOL_PATH", "$(myrrha_ssa_DB)")

epicsEnvSet(CAB_NUM, "3")
epicsEnvSet(IOC_NUM, "100")
epicsEnvSet(CAB_SSA, "CAB-$(CAB_NUM)")
epicsEnvSet(CAB_IOC, "CAB-$(IOC_NUM)")
epicsEnvSet(DISCIPLINE, "RFS")

epicsEnvSet(PREFIX_CGD, "$(CAB_SSA):$(DISCIPLINE)-CGD")
epicsEnvSet(SLAVE_ID, "0")
epicsEnvSet(ECAT_FREQ, "100")

loadIocsh("cab.iocsh")

