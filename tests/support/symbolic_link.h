/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/symbolic_link.h
 * Purpose: Create Windows symbolic-link fixtures without treating unexpected failures as skips.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SUPPORT_SYMBOLIC_LINK_H
#define UMICOM_TEST_SUPPORT_SYMBOLIC_LINK_H
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdio.h>

/* Fixture capability is separate from the behaviour under test. Return one
 * for a created link, zero only for a known unavailable capability, and minus
 * one for invalid input or an unexpected filesystem failure. Callers must
 * assert against minus one; otherwise a regression could become a skip. */
static inline int UmiTestWindowsSymbolicLink(const char *alias, const char *target, int directory)
{
    wchar_t aliasWide[4096], targetWide[4096];
    if (alias == NULL || target == NULL ||
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, alias, -1, aliasWide, 4096) == 0 ||
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, target, -1, targetWide, 4096) == 0) {
        fprintf(stderr, "ERROR: invalid UTF-8 path or oversized symbolic-link fixture path.\n");
        return -1;
    }
    DWORD flags = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0U;
    if (CreateSymbolicLinkW(aliasWide, targetWide, flags | 2U)) return 1;
    DWORD error = GetLastError();
    /* Older Windows versions do not accept the unprivileged creation flag.
     * Retrying without it still honours the process's existing privileges. */
    if (error == ERROR_INVALID_PARAMETER) {
        if (CreateSymbolicLinkW(aliasWide, targetWide, flags)) return 1;
        error = GetLastError();
    }
    if (error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_NOT_SUPPORTED ||
        error == ERROR_INVALID_FUNCTION) {
        fprintf(stderr, "SKIP: Windows symbolic-link fixtures require Developer Mode, "
                        "link privileges and filesystem support (error %lu).\n", (unsigned long)error);
        return 0;
    }
    fprintf(stderr, "ERROR: symbolic-link fixture creation failed (Windows error %lu).\n",
            (unsigned long)error);
    return -1;
}
#endif
#endif
