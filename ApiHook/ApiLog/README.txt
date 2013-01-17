API logging DLL for KernelEx Core API hook infrastructure

Make sure to disable API extensions for this module or else the DLL
will try to hooks its own imports.

By default you have to rename the DLL to kexApiHook.dll and place it either in C:\Windows\KernelEx or somewhere in DLL search path.
Alternatively you can set the 'ApiHookPath' value under 'HKEY_LOCAL_MACHINE\Software\KernelEx' to full path to the DLL.
Search path takes precedence over other methods.

The DLL can output logs using different methods, set 'KEXAPILOG_OUTPUT' environment variable to change:

file - API logs are written to a file named as module but with '.log' extension (default)
window - API logs are output to DebugWindow application
debug - API logs are output via OutputDebugString API to system debugger

The DLL will also try to load the file called 'signature.dat' in the directory where the DLL is located.
This file contains API function signatures for parameter logging.

Syntax of the file:

[MODULENAME.DLL]
ApiName=format

where format is a sequence of letters from printf format specifiers:
p - pointer in the form of: 0x1234abcd
d - decimal number
x - hexadecimal number
s - ansi string
S - unicode string
f - float

Example:

[KERNEL32.DLL]
GetProcAddress=ps


Errors related to API hook DLL loading are written to debug output.
