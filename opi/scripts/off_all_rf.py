from org.csstudio.display.builder.runtime.script import ScriptUtil, PVUtil
from java.lang import Exception as JavaException

cab_num = widget.getDisplayModel().getPropertyValue("macros").getValue('CAB_NUM')
discipline = widget.getDisplayModel().getPropertyValue("macros").getValue('DISCIPLINE')

pv_fmt = 'CAB-{}:{}-Amp-{{}}:RFOff-Cmd'.format(cab_num, discipline)
reg_ssa = ['A', 'B']

for pv_ssa in [pv_fmt.format(reg) for reg in reg_ssa] :
    print(pv_ssa)
    try :
        PVUtil.writePV(pv_ssa, 1, 1000)
    except JavaException as je:
        # Catches Java-level issues (Timeout, PV not found, Network error)
        print("Java Error: Failed to write to " + pv_ssa)
        print("Details: " + str(je.getMessage()))
 
