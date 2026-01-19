from org.csstudio.opibuilder.scriptUtil import PVUtil

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
for p in pvs:
    name = p.getName()
    value = PVUtil.getLong(p)
    for k, v in error_values.items():
        if name.endswith(k) and value == v:
            have_error = True
            break
    if have_error:
        break

if have_error:
    widget.setValue(error)
elif have_warning:
    widget.setValue(warning)
else:
    widget.setValue(no_error)
