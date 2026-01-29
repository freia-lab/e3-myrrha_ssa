from org.csstudio.display.builder.runtime.script import ScriptUtil, PVUtil

# Use ScriptUtil to get the actual PV object
pv = ScriptUtil.getPrimaryPV(widget)

if not pv:
    exit

no_error = 0
warning = 1
error = 2

# For most status PVs, 0 is the value that means "error", but not for all, so
# we need a mapping.
error_values = {
    ':ExtIntlk1': 0,
    ':ExtIntlk2': 0,
    ':WaterFlowInRange': 0,
    ':WaterFlowStat': 0,
    ':Leak': 0,
    ':DoorFrontStat': 0,
    ':DoorRearStat': 0,
    ':LoadWaterFlowStat': 0,
    ':RFLineTStat': 0,
    ':ExtT1Warn': 0,
    ':ExtT2Warn': 0,
    ':IntTWarn': 0,
    ':ExtT1Err': 0,
    ':ExtT2Err': 0,
    ':IntTErr': 0,
    ':PSUVoltStat': 0,
    ':AllCommStat': 0,
}

have_warning = False
have_error = False
print (pvs)
for p in pvs:
    name = p.getName()
    print(name)
    value = PVUtil.getLong(p)
    for k, v in error_values.items():
        if name.endswith(k) and value == v:
            have_error = True
            break
    if have_error:
        break

if have_error:
    # Now you can use PVUtil to write or read
    PVUtil.writePV(pv.getName(),error, 1)
    print("Error")
elif have_warning:
    PVUtil.writePV(pv.getName(), warning, 1)
    print("Warning")
else:
    print("OK")
    PVUtil.writePV(pv.getName(), no_error, 1)
