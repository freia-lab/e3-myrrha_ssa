require myrrha_ssa
require ecat2

epicsEnvSet(IOC_NUM, "1")
epicsEnvSet(CAB_NUM, "190")
epicsEnvSet(IOC_CAB_NUM, "100")
epicsEnvSet(DISCIPLINE, "RFS")

epicsEnvSet(CAB_SSA, "CAB-$(CAB_NUM)")
epicsEnvSet(CAB_IOC, "CAB-$(IOC_CAB_NUM)")

epicsEnvSet(PREFIX_ECAT, "$(CAB_IOC):$(DISCIPLINE)-IOC")
iocshLoad("$(myrrha_ssa_DIR)ecat.iocsh", "IOC_NUM=$(IOC_NUM),ECAT_FREQ=100")

epicsEnvSet(PREFIX_CGD, "$(CAB_SSA):$(DISCIPLINE)-CGD")
epicsEnvSet(NUM_PSU, "5")
epicsEnvSet(NUM_SSM, "8")
epicsEnvSet(NUM_AMP, "2")
iocshLoad("$(myrrha_ssa_DIR)cab.iocsh", "SLAVE_ID=0")

epicsEnvSet(PREFIX_AMP, "$(CAB_SSA):$(DISCIPLINE)-Amp")
iocshLoad("$(myrrha_ssa_DIR)amp.iocsh", "AMP_CHAR=A,NUM_SSM=4,AMP_SSM='1\\\,3\\\,5\\\,7'")
iocshLoad("$(myrrha_ssa_DIR)amp.iocsh", "AMP_CHAR=B,NUM_SSM=4,AMP_SSM='2\\\,4\\\,6\\\,8'")

epicsEnvSet(PREFIX_PSU, "$(CAB_SSA):$(DISCIPLINE)-PSU")
iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PSU_NUM=1")
iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PSU_NUM=2")
iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PSU_NUM=3")
iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PSU_NUM=4")
iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PSU_NUM=5")

epicsEnvSet(PREFIX_SSM, "$(CAB_SSA):$(DISCIPLINE)-SSM")
epicsEnvSet(IP_ADDR_BASE, "10.100")
epicsEnvSet(POLL_MS, "1000")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=1")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=2")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=3")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=4")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=5")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=6")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=7")
iocshLoad("$(myrrha_ssa_DIR)ssm.iocsh", "SSM_NUM=8")

