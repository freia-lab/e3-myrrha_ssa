# @field PREFIX_ECAT
# @type STRING
# Prefix used for record names

# @field ECAT_FREQ
# @type STRING
# EtherCAT update frequency in Hz

# @field IOC_NUM
# @type STRING
# The number of the IOC process in this cabinet

loadIocsh("ecat.iocsh", "PREFIX_ECAT=$(PREFIX_ECAT),ECAT_FREQ=$(ECAT_FREQ),IOC_NUM=$(IOC_NUM)")
