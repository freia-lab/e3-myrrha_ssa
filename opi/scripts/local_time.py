from org.csstudio.opibuilder.scriptUtil import PVUtil
import time
import sys

value = PVUtil.getLong(pvs[0])
my_time = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(value))
value = PVUtil.getLong(pvs[1])
my_time = my_time + " " + str(value/1000) + u"\u03BCs"
widget.setValue(my_time)
