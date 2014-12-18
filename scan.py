#confirmed 7 digitals are formed as 28 bit , 0xFFFFFFF in hex, not just any ACSII number or letter.
#so i will pick lower 16 bit, that is 65535 values, enough for our usage.
#bar scaner is just like a keyboard, put what is scaned and adding a \n.
#wistron sn standard is like: "555M009D01G450000028J0SA", i pick "0028"
#usage: python programming.py com_port_number
#plug arduino before executing programming.py

import serial
import time

def programming(comx):
	""" plug in arduino before executing programming,py
usage: under scripts's folder, open cmd, type: python scan.py comx
	"""
	ser = serial.Serial(comx - 1)
	while True:
		t = time.localtime()
		print(t.tm_year, '/', t.tm_mon, '/', t.tm_mday, ' ',
		str(t.tm_hour).zfill(2), ':', str(t.tm_min).zfill(2),
		':', str(t.tm_sec).zfill(2), end = ' ', sep = '')
		
		sn = input(" scan SN: ")
		sn_send = bytes.fromhex(sn[16:20])
		if ser.write(sn_send) != 2:
			print("SN is incorrect! Check!")
			ser.close()
			break
		while ser.inWaiting() == 0:
			pass
		else:
			if ser.read() != b'\x50':
				print("----------------------------------------------------NG")
				break
			else:
				print("----------------------------------------------------PASS")
				
				
if __name__ == "__main__":
	import sys
	programming( int( sys.argv[1] ) )
