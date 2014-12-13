#confirmed 7 digitals are formed as 28 bit , 0xFFFFFFF in hex, not just any ACSII number or letter.
#so i will pick lower 16 bit, that is 65535 values, enough for our usage.
#bar scaner is just like a keyboard, put what is scaned and adding a \n.
#wistron sn standard is like: "555M009D01G450000028J0SA"

import time
import serial

t = time.localtime()
print(t.tm_mon, t.tm_day, t.tm_hour, t.tm_min, t.tm_sec, end = ': ')
sn = input("scan SN: ")[ 15 : 19]
bytes.fromhex(sn)


if "__name__" == "__main__":
        __main__()
