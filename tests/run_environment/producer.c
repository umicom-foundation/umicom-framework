/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/producer.c
 * PURPOSE: Report a controlled child variable using native Unicode environment access on Windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif
int main(void)
{
#ifdef _WIN32
    wchar_t wide[1024];
    SetLastError(ERROR_SUCCESS);
    DWORD length = GetEnvironmentVariableW(L"UMICOM_RUN_VALUE", wide, 1024U);
    if (length == 0U && GetLastError() == ERROR_ENVVAR_NOT_FOUND)
        return 3;
    if (length >= 1024U)
        return 4;
    wide[length] = L'\0';
    char value[4096];
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, value, (int)sizeof value, NULL,
                            NULL) <= 0)
        return 5;
#else
    const char *value = getenv("UMICOM_RUN_VALUE");
    if (value == NULL)
        return 3;
#endif
    printf("received:[%s]\n", value);
    return fflush(stdout) == 0 ? 0 : 6;
}
