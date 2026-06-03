import ctypes, sys
from ctypes import wintypes

path = r"c:\Users\Kaas\Documents\Dev\navi\native\cm93\build\cm93_wrapper.dll"
print('Checking:', path)

k = ctypes.WinDLL('kernel32', use_last_error=True)
LoadLibraryA = k.LoadLibraryA
LoadLibraryA.argtypes = [ctypes.c_char_p]
LoadLibraryA.restype = wintypes.HMODULE
GetProcAddress = k.GetProcAddress
GetProcAddress.argtypes = [wintypes.HMODULE, ctypes.c_char_p]
GetProcAddress.restype = ctypes.c_void_p
FreeLibrary = k.FreeLibrary
FreeLibrary.argtypes = [wintypes.HMODULE]
FreeLibrary.restype = wintypes.BOOL

h = LoadLibraryA(path.encode('utf-8'))
if not h:
    err = ctypes.get_last_error()
    print('LoadLibrary failed, GetLastError =', err)
    sys.exit(1)

symbols = [b'cm93_init', b'cm93_render', b'cm93_free_buffer', b'cm93_version']
for s in symbols:
    addr = GetProcAddress(h, s)
    print(s.decode('utf-8'), 'FOUND' if addr else 'NOT FOUND')

FreeLibrary(h)
print('Done')
