/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/utf8_entry.inc
 * PURPOSE: Let native Windows fixtures observe their actual Unicode arguments.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
/* This shared entry point owns its C runtime and Windows declarations. Some
 * GTK-only fixtures have no other reason to include windows.h or stdlib.h.
 * Use the SDK declarations, not hand-written prototypes or replacement flags.
 * A lean Windows include avoids introducing the older Winsock header here. */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
int wmain(int argc, wchar_t **wideArguments)
{
    /* Binary streams preserve the exact UTF-8 bytes emitted by the fixture;
     * a stream-mode failure retains the existing setup-failure exit code. */
    if (_setmode(_fileno(stdout), _O_BINARY) < 0 ||
        _setmode(_fileno(stderr), _O_BINARY) < 0) return 125;
    /* Zero-initialised slots make the common cleanup path safe even when
     * allocation or conversion fails part-way through the argument array. */
    char **arguments = calloc((size_t)argc + 1U, sizeof(*arguments));
    if (arguments == NULL) return 125;
    int result = 125;
    /* Measure in output bytes, allocate that exact size, then convert with
     * the same strict UTF-8 policy. The -1 input length includes the terminator;
     * malformed UTF-16 must fail rather than silently changing a test path. */
    for (int i = 0; i < argc; ++i) {
        int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1, NULL, 0, NULL, NULL);
        if (bytes <= 0 || (arguments[i] = malloc((size_t)bytes)) == NULL) goto done;
        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1,
                arguments[i], bytes, NULL, NULL) != bytes) goto done;
    }
    /* Publish arguments only after every conversion succeeds. ProgramMain
     * borrows this array for the duration of the call and keeps its exit code. */
    result = ProgramMain(argc, arguments);
done:
    /* Release every successfully allocated argument, including partial setup.
     * free(NULL) is safe for the slots that were never populated. */
    for (int i = 0; i < argc; ++i) free(arguments[i]);
    free(arguments);
    return result;
}
#else
int main(int argc, char **arguments) { return ProgramMain(argc, arguments); }
#endif
