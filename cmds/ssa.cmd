require myrrha_ssa,master
require ecat2,3.0.5

epicsEnvSet(CAB_NUM, "3")
epicsEnvSet(IOC_NUM, "100")
epicsEnvSet(CAB_SSA, "CAB-$(CAB_NUM)")
epicsEnvSet(CAB_IOC, "CAB-$(IOC_NUM)")
epicsEnvSet(DISCIPLINE, "RFS")

epicsEnvSet(PREFIX_CGD, "$(CAB_SSA):$(DISCIPLINE)-CGD")
epicsEnvSet(SLAVE_ID, "0")
epicsEnvSet(ECAT_FREQ, "100")

loadIocsh("cab.iocsh")

epicsEnvSet(PREFIX_AMP, "$(CAB_SSA):$(DISCIPLINE)-Amp")

loadIocsh("amp.iocsh", "AMP_CHAR=A")

epicsEnvSet(PREFIX_PSU, "$(CAB_SSA):$(DISCIPLINE)-PSU")

loadIocsh("psu.iocsh", "PSU_NUM=2")

epicsEnvSet(PREFIX_SSM, "$(CAB_SSA):$(DISCIPLINE)-SSM")
epicsEnvSet(IP_ADDR_BASE, "10.100")
epicsEnvSet(POLL_MS, "1000")

loadIocsh("ssm.iocsh", "SSM_NUM=1")
