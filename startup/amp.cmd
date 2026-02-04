# @field PREFIX_AMP
# @type STRING
# Prefix used for record names

# @field PREFIX_CGD
# @type STRING
# Prefix used for CGD records

# @field AMP_CHAR
# @type STRING
# The letter of the amplifier (A, B, C or D)

# @field NUM_SSM
# @type STRING
# The number of SSMs in this amplifier

# @field AMP_SSM
# @type STRING
# The list of SSMs in this amplifier, separated by commas

loadIocsh("$(myrrha_ssa_DIR)amp.iocsh", "PREFIX_AMP=$(PREFIX_AMP),PREFIX_CGD=$(PREFIX_CGD),AMP_CHAR=$(AMP_CHAR),NUM_SSM=$(NUM_SSM),AMP_SSM=$(AMP_SSM)")
