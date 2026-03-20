require ecmccfg


# 1. Add EtherCAT master
iocshLoad ${ecmccfg_DIR}startup.cmd, "ECMC_VER=8.0.2+0,EC_RATE=100,MODE=DAQ,TMP_DIR=/tmp"


# 2. Add your slave
${SCRIPTEXEC} ${ECMC_CONFIG_ROOT}/addSlave.cmd,   "HW_DESC=CIFX_REECS, SLAVE_ID=0"

# 4. Finalize whole EtherCAT scan + PDO assignment
${SCRIPTEXEC} ${ECMC_CONFIG_ROOT}/applyConfig.cmd

# Finally start ECMC runtime
#ecmcConfigOrDie("Cfg.Start=1")
