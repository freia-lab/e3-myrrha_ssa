require ecmccfg


# 1. Add EtherCAT master
iocshLoad ${ecmccfg_DIR}startup.cmd, "ECMC_VER=8.0.2+0,EC_RATE=100,MODE=DAQ,TMP_DIR=/tmp"


# 2. Add your slave
${SCRIPTEXEC} ${ECMC_CONFIG_ROOT}/addSlave.cmd,   "HW_DESC=CIFX_REECS, SLAVE_ID=0"

# 4. Finalize whole EtherCAT scan + PDO assignment
${SCRIPTEXEC} ${ECMC_CONFIG_ROOT}/applyConfig.cmd
# ================================================================
# CIFX_post.cmd — AFTER Cfg.EcApplyConfig(1)
# Correct mapping for two PDOs per direction.
# ================================================================

on error halt

# -----------------------------
# OUT direction (RxPDO) → SM2
# dir = 1  (write to slave)
# -----------------------------

# OUT p0: 200 bytes (PDO 1600)
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm2.p0.e0, 200, 1, U8, ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_OUT_P0)"

# OUT p1: 50 bytes (PDO 1601)
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm2.p1.e0,  50, 1, U8, ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_OUT_P1)"

# -----------------------------
# IN direction (TxPDO) → SM3
# dir = 2  (read from slave)
# -----------------------------

# IN p0: 200 bytes (PDO 1A00)
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm3.p0.e0, 200, 2, U8, ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_IN_P0)"

# IN p1: 50 bytes (PDO 1A01)
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm3.p1.e0,  50, 2, U8, ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_IN_P1)"

# -----------------------------
# Load DB waveforms
# -----------------------------
#dbLoadTemplate("${ECMC_CONFIG_DB}/CIFX_waveforms.substitutions")

${SCRIPTEXEC} ${ecmccfg_DIR}slaveVerify.cmd
# OUT MemMap (SM2)
epicsEnvSet("ECMC_SIZE","200")
epicsEnvSet("ECMC_EC_SM","2")
epicsEnvSet("ECMC_EC_DIR","1")
epicsEnvSet("ECMC_EC_PDO","0x1600")
epicsEnvSet("ECMC_EC_ENTRY","0x2000")
epicsEnvSet("ECMC_EC_TYPE","U8")
epicsEnvSet("ECMC_EC_KEY","sm2.p0.e")
ecmcEpicsEnvSetCalc(FOR_LAST_IDX,"200-1")
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm2.p0.e0,200,2,U8,ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_OUT)"
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm2.p1.e0,50,2,U8,ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_OUT)"

# IN MemMap (SM3)
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm3.p0.e0,200,2,U8,ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_IN)"
#ecmcConfigOrDie "Cfg.EcAddMemMapDT(ec0.s${ECMC_EC_SLAVE_NUM}.sm3.p1.e0,50,2,U8,ec0.s${ECMC_EC_SLAVE_NUM}.mm.RAW_IN)"

# Finally start ECMC runtime
#ecmcConfigOrDie("Cfg.Start=1")

# Load Db
#dbLoadTemplate("${ECMC_CONFIG_DB}/CIFX_REECS_waveforms.substitutions")
