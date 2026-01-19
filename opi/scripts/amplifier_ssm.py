from org.csstudio.opibuilder.scriptUtil import PVUtil, DataUtil

ssms = PVUtil.getLongArray(pvs[0])
name_prefix = 'SSM_Container_'
to_remove = []

for wgt in widget.getChildren():
    name = wgt.getWidgetModel().getName()
    if not name.startswith(name_prefix):
        continue
    ssm_num = name.lstrip(name_prefix)
    if int(ssm_num) in ssms:
        macros = DataUtil.createMacrosInput(True)
        macros.put("SSM_NUM", ssm_num)
        wgt.setPropertyValue('macros', macros)
        wgt.setPropertyValue('opi_file', 'SSA_Ssm_amp.opi')
    else:
        to_remove.append(name)

for name in to_remove:
    widget.removeChildByName(name)
