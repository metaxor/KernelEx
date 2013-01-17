#include "main.h"

BOOL ErrorCodeToString(DWORD dwErrorCode, LPVOID lpMsgBuf)
{
    return FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) lpMsgBuf,
        0, NULL);
}
