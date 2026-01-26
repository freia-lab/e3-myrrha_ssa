
e3-myrrha_ssa  
======
ESS Site-specific EPICS module : myrrha_ssa

PROBLEMS:
=========
Missing code for function: toEpochNs but it looks that it's not so important.
It is used to create records:
record(int64in, "$(PREFIX)-$(SSM_NUM):Timestamp")
record(int64in, "$(PREFIX)-$(SSM_NUM):Datetime")
that are not used by any other records (other that aSub records writing to them)

Uncertain if the source code for the raw TCP communication is up to-date. But
it is mostly used for the data dump from SSM and is not so importanf for FREIA
operation.

-----
OPIs
------
Replace the python scripts for multi-states LEDS with calc records in the IOC