BOOL CreateProcessInDesktop(HWINSTA hWinSta,
                             HDESK hDesktop,
                             LPCTSTR lpApplicationName,
                             LPTSTR lpCommandLine,
                             LPSECURITY_ATTRIBUTES lpProcessAttributes,
                             LPSECURITY_ATTRIBUTES lpThreadAttributes,
                             BOOL bInheritHandles,
                             DWORD dwCreationFlags,
                             LPVOID lpEnvironment,
                             LPCTSTR lpCurrentDirectory,
                             LPSTARTUPINFO lpStartupInfo,
                             LPPROCESS_INFORMATION lpProcessInformation);

BOOL ErrorCodeToString(DWORD dwErrorCode, LPVOID lpMsgBuf);