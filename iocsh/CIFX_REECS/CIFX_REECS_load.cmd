# CIFX_REECS_load.cmd
# Loads your YAML cleanly using ECMC’s YAML loader

on error halt

# Load the YAML through ECMC -- this is the CORRECT loader
ecmcConfig "Cfg.LoadYaml('/home/konrad/modules/e3-myrrha_ssa/iocsh/CIFX_REECS/ethercat_slave.yaml')"
