from org.csstudio.display.builder.runtime.script import ScriptUtil, PVUtil
#from pv_interface1 import pvConnectAndSetList

cab_num = widget.getDisplayModel().getPropertyValue("macros").getValue('CAB_NUM')
discipline = widget.getDisplayModel().getPropertyValue("macros").getValue('DISCIPLINE')
prefix = 'CAB-{}:{}-'.format(cab_num, discipline)
pvs = ScriptUtil.getPrimaryPV(widget)
num_ssm = PVUtil.getLong(pvs)
fmt = prefix + 'SSM-{}:Rst-Cmd'
print(fmt)
print(num_ssm)
print([fmt.format(i) for i in range(1, num_ssm+1)], [1] * num_ssm)

#pvConnectAndSetList([fmt.format(i) for i in range(1, num_ssm+1)], [1] * num_ssm)
