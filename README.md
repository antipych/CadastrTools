Build tips

1. Setup ObjectARX SDK and add additional qmake argument "ARXSDK"

example:
	for AutoCad 2014 "ARXSDK = D:\Autodesk\Autodesk_ObjectARX_2014_Win_64_and_32Bit"
	for AutoCad 2012 "ARXSDK = D:\Autodesk\ObjectARX_2012"

2. For debuging release build add additional qmake argument "CONFIG+= RELEASE_WITH_DEBUGINFO"