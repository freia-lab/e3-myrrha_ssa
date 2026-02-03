from org.csstudio.display.builder.runtime.script import PVUtil, ScriptUtil
import time

# 1. Get the list of PVs assigned to this script
# In Phoebus, 'pvs' is still available if you defined them in the script properties
try:
    # Get values using Phoebus PVUtil
    raw_seconds = PVUtil.getLong(pvs[0])
    raw_micros = PVUtil.getLong(pvs[1])

    # 2. Format the Date/Time
    formatted_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(raw_seconds))

    # 3. Handle Microseconds and the Greek Mu symbol (u"\u03BC")
    # Using format() is cleaner for Jython string concatenation
    micro_val = str(raw_micros / 1000)
    final_string = u"{} {} {}\u03BCs".format(formatted_time, micro_val, "")

    # 4. Update the widget. In Phoebus, most text widgets use the "text" property.
    widget.setPropertyValue("text", final_string)

except Exception as e:
    # Log errors to the Phoebus console for debugging
    print("Error in timestamp script: " + str(e))
    
