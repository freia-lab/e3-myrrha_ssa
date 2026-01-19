# @field PREFIX_SSM
# @type STRING
# Prefix used for record names

# @field IP_ADDR_BASE
# @type STRING
# The start of the IP address, e.g. 10.100

# @field CAB_NUM
# @type STRING
# The number of the cabinet

# @field SSM_NUM
# @type STRING
# The number of the module

# @field POLL_MS
# @type STRING
# Modbus polling rate in milliseconds

loadIocsh("ssm.iocsh", "PREFIX_SSM=$(PREFIX_SSM),IP_ADDR_BASE=$(IP_ADDR_BASE),CAB_NUM=$(CAB_NUM),SSM_NUM=$(SSM_NUM),POLL_MS=$(POLL_MS)")
