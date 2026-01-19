from org.csstudio.opibuilder.scriptUtil import PVUtil
from pv_interface import pvConnectAndGetList as getList, \
    pvConnectAndSetList as setList

num_ssms = PVUtil.getLong(pvs[1])
this_ssm = int(widget.getMacroValue('SSM_NUM'))
other_ssms = [i for i in range(1, num_ssms + 1) if i != this_ssm]

pv_fmt = 'CAB-{}:{}-SSM-{{}}:{{}}StatTrigMask-Sel'.format(
    widget.getMacroValue('CAB_NUM'),
    widget.getMacroValue('DISCIPLINE'))
commit_pv_fmt = 'CAB-{}:{}-SSM-{{}}:TrigMaskWrite-Cmd.PROC'.format(
    widget.getMacroValue('CAB_NUM'),
    widget.getMacroValue('DISCIPLINE'))

registers = ['SSM', 'Pall', 'Rst', 'TWarn', 'TLim', 'Pwr',
             'VoltLim', 'CurrLim', 'Ref']

this_ssm_values = getList([pv_fmt.format(this_ssm, reg) for reg in registers])

for ssm in other_ssms:
    setList([pv_fmt.format(ssm, reg) for reg in registers], this_ssm_values)

setList([commit_pv_fmt.format(ssm) for ssm in range(1, num_ssms + 1)],
        [1] * num_ssms)
