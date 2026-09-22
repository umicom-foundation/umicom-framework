/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/utf8_entry.inc
 * PURPOSE: Let native Windows fixtures observe their actual Unicode arguments.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
int wmain(int argc, wchar_t **wideArguments)
{
    if (_setmode(_fileno(stdout), _O_BINARY) < 0 ||
        _setmode(_fileno(stderr), _O_BINARY) < 0) return 125;
    char **arguments = calloc((size_t)argc + 1U, sizeof(*arguments));
    if (arguments == NULL) return 125;
    int result = 125;
    for (int i = 0; i < argc; ++i) {
        int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1, NULL, 0, NULL, NULL);
        if (bytes <= 0 || (arguments[i] = malloc((size_t)bytes)) == NULL) goto done;
        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1,
                arguments[i], bytes, NULL, NULL) != bytes) goto done;
    }
    result = ProgramMain(argc, arguments);
done:
    for (int i = 0; i < argc; ++i) free(arguments[i]);
    free(arguments);
    return result;
}
#else
int main(int argc, char **arguments) { return ProgramMain(argc, arguments); }
#endif
