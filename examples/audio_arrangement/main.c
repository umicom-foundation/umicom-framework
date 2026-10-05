/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/audio_arrangement/main.c
 * PURPOSE: Translate native command arguments once and delegate audio work to the shared Framework command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "command.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdlib.h>
/* Windows starts with UTF-16 arguments. Convert each complete value to UTF-8
 * before the shared file APIs see it; an ANSI code page must not change paths.
 * No shell expression is reconstructed and malformed UTF-16 is refused. */
int wmain(int argc, wchar_t **wide)
{
    if (argc < 1 || argc > 5)
        return 2;
    char *argv[6] = {0};
    int result = 1;
    for (int i = 0; i < argc; ++i)
    {
        int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, NULL, 0, NULL, NULL);
        if (count < 1 || count > 16384)
            goto done;
        argv[i] = malloc((size_t)count);
        if (argv[i] == NULL || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, argv[i], count,
                                                   NULL, NULL) != count)
            goto done;
    }
    result = UmiAudioArrangementCommand(argc, argv);
done:
    for (int i = 0; i < argc; ++i)
        free(argv[i]);
    return result;
}
#else
int main(int argc, char **argv) { return UmiAudioArrangementCommand(argc, argv); }
#endif
