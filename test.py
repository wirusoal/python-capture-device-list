import cv2
import ctypes
lib = ctypes.CDLL('device.dll')
class devices_list(ctypes.Structure):
    _fields_ = [('id', ctypes.c_int),('name', ctypes.c_char_p),('path', ctypes.c_char_p)]

class devices_count(ctypes.Structure):
    _fields_ = [('countdevice', ctypes.c_int),
    ('devices_list', devices_list*10)]

pfun = lib.devices
pfun.restype = devices_count
returnfun = pfun()
for i in xrange(0,returnfun.countdevice):
    print "id:",returnfun.devices_list[i].id
    print "name",returnfun.devices_list[i].name
    print "path",returnfun.devices_list[i].path

idCamera = raw_input('id camera: ') 
cap = cv2.VideoCapture(int(idCamera))

while(True):
    ret, frame = cap.read()
    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()