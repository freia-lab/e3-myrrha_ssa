############################################################
# ECMC / EtherCAT configuration for Hilscher CIFX RE/ECS
# 250 bytes OUT, 250 bytes IN
# Master frequency: 100 Hz
############################################################

########################################################
# Create ECMC asyn driver
########################################################
# port name: ECMC
# parameter table size: 2000
# thread priority: 50
# disable auto connect: 0
# default param sample rate: 10 ms

ecmcAsynPortDriverConfigure "ECMC" 2000 50 0 10

ecmcConfigOrDie "Cfg.EcEnablePrintouts(1)"
ecmcConfigOrDie "Cfg.EcSetDiagnostics(1)"

#ecmcConfigOrDie "createMasterSlaveSM(0,'cifx','master','slave0')"

#ecmcConfigOrDie "Cfg.EcAddDomain(1,0)"
#ecmcConfigOrDie "Cfg.EcAddDomain(1,250)"

# Alias = 0, position = 0
#ecmcConfigOrDie "Cfg.EcAddSlave(0,0)"

# Outputs
#ecmcForLoop cmds/add_out.cmd "" IDX 1 200 1
#ecmcForLoop cmds/add_out2.cmd "" IDX 1 50 1

# Inputs
#ecmcForLoop cmds/add_in.cmd "" IDX 1 200 1
#ecmcForLoop cmds/add_in2.cmd "" IDX 1 50 1

#ecmcConfigOrDie "Cfg.EcApplyConfig()"
#ecmcConfigOrDie "Cfg.Start()"

