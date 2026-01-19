from org.csstudio.opibuilder.scriptUtil import ConsoleUtil
from pv_interface import pvCreate as pvCreate, \
                         pvGet as pvGet, \
                         pvSet as pvSet


cab_num = widget.getMacroValue('CAB_NUM')
discipline = widget.getMacroValue('DISCIPLINE')
pv_fmt = 'CAB-{}:{}-Amp-{{}}:RFOff-Cmd'.format(cab_num, discipline)
reg_ssa = ['A', 'B', 'C', 'D']

for pv_ssa in [pv_fmt.format(reg) for reg in reg_ssa] :
    pv = pvCreate(pv_ssa)
    try :
        pvGet(pv)
        pvSet(pv, 1)       
    except RuntimeError:
        ConsoleUtil.writeInfo("PV "+ pv.getName() + " not defined");
    pv.stop()
