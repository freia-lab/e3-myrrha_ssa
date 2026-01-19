from org.csstudio.opibuilder.scriptUtil import PVUtil
from pv_interface import pvConnectAndSetList

cab_num = widget.getMacroValue('CAB_NUM')
discipline = widget.getMacroValue('DISCIPLINE')
prefix = 'CAB-{}:{}-'.format(cab_num, discipline)

num_ssm = PVUtil.getLong(pvs[0])
fmt = prefix + 'SSM-{}:Rst-Cmd'
pvConnectAndSetList([fmt.format(i) for i in range(1, num_ssm+1)], [1] * num_ssm)
