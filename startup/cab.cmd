# @field PREFIX_CGD
# @type STRING
# Prefix used for record names

# @field CAB_NUM
# @type STRING
# The number of the cabinet

# @field SLAVE_ID
# @type STRING
# EtherCAT slave ID

# @field NUM_PSU
# @type STRING
# The total number of PSUs installed

# @field NUM_SSM
# @type STRING
# The total number of SSMs installed

# @field NUM_AMP
# @type STRING
# The total number of amplifiers installed

loadIocsh("cab.iocsh", "PREFIX_CGD=$(PREFIX_CGD),CAB_NUM=$(CAB_NUM),SLAVE_ID=$(SLAVE_ID),NUM_PSU=$(NUM_PSU),NUM_SSM=$(NUM_SSM),NUM_AMP=$(NUM_AMP)")
