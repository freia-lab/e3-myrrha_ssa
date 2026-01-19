# Overrides network vars for SSM simulator

epicsEnvSet(SSM_ADDR, "localhost")
epicsEnvSet(MODBUS_PORT, 5020)
epicsEnvSet(BUFFER_PORT, 5030)
< "cmds/ssa.cmd"
