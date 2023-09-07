To Help anyone else who has a similar problem:
Here's the code I used with NSIS version 1.81. The date is stored in $R1 and $R2 and these values can be written to the registry or a file or whatever. The 2 variables $R1 and $R2 form the 2 variables of a win32 FILETIME structure.Create a FILETIME structure in your C++ program and initialize it with the values of these 2 variables (wherever you stored them). The FILETIME struct can then be manipulated to turn it into a SYSTEMTIME structure or a CTime object.

;get current time by creating a temp file and reading it's time stamp
;make a new file in the temp directory
SetOutPath "$TEMP"
File "new file"
SetOutPath "$INSTDIR"
;get it's time stamp
GetFileTime "$TEMP\new file" $R1 $R2
; $R1 is high dword of current time
; $R2 is low dword of current time
;delete our new file (it's purpose has been served)
Delete "$TEMP\new file"





  GetFileTimeLocal "..\Release\Cropper.exe" $1 $0
  System::Int64Op $1 * 0x100000000
  Pop $1
  System::Int64Op $1 + $0
  Pop $0
  System::Call "*(&i2, &i2, &i2, &i2, &i2, &i2, &i2, &i2) i .r1"
  System::Call "Kernel32::FileTimeToSystemTime(*l r0, i r1)"
  System::Call "Kernel32::GetDateFormatA(i 0, i 0, i r1, t 'yyyyMMdd', t .r0, i ${NSIS_MAX_STRLEN})"
  Pop $0
  DetailPrint 'The version is: $0'
  
  
  
FileOpen $0 $EXEDIR\bla.exe r
System::Call 'kernel32::GetFileTime(i, *l, *l, *l) i (r0, .r1, .r2, .r3)'
FileClose $0
FileOpen $0 $EXEDIR\uninst.exe a
System::Call 'kernel32::SetFileTime(i, *l, *l, *l) i (r0, r1, r2, r3)'
FileClose $0



