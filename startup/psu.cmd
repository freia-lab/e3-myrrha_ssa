# @field PREFIX_PSU
# @type STRING
# Prefix used for record names

# @field PREFIX_CGD
# @type STRING
# Prefix used for CGD records

# @field PSU_NUM
# @type STRING
# The number of the PSU

iocshLoad("$(myrrha_ssa_DIR)psu.iocsh", "PREFIX_PSU=$(PREFIX_PSU),PREFIX_CGD=$(PREFIX_CGD),PSU_NUM=$(PSU_NUM)")
