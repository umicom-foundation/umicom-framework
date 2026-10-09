/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/producer.c
 * PURPOSE: Provide deterministic native stdin, EOF, output and environment scenarios.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
int main(int argc, char **argv)
{
#ifdef _WIN32
    (void)_setmode(_fileno(stdin), _O_BINARY);
    (void)_setmode(_fileno(stdout), _O_BINARY);
    (void)_setmode(_fileno(stderr), _O_BINARY);
#endif
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    if (strcmp(mode, "wait") == 0)
    {
        umi_thread_sleep_ms(30000U);
        return 0;
    }
    if (strcmp(mode, "environment") == 0)
    {
        const char *value = getenv("UMICOM_CONSOLE_FIXTURE_VALUE");
        printf("value=%s\n", value != NULL ? value : "(absent)");
        return 0;
    }
    if (strcmp(mode, "large") == 0)
    {
        for (size_t i = 0U; i < 140000U; ++i)
            if (putchar('x') == EOF)
                return 3;
        fputs("\nlast stdout\n", stdout);
        for (size_t i = 0U; i < 10000U; ++i)
            if (fputc('e', stderr) == EOF)
                return 3;
        fputs("\nlast stderr\n", stderr);
        return 0;
    }
    if (strcmp(mode, "exit") == 0)
        return 23;
    if (strcmp(mode, "closed-output") == 0)
    {
        (void)fclose(stdout);
        char input[128];
        size_t total = 0U, received;
        while ((received = fread(input, 1U, sizeof input, stdin)) != 0U)
            total += received;
        fprintf(stderr, "input bytes=%zu\\n", total);
        return ferror(stdin) ? 4 : 0;
    }
    if (strcmp(mode, "echo") != 0 && strcmp(mode, "nul") != 0)
        return 2;
    fputs("READY\n", stdout);
    fflush(stdout);
    if (strcmp(mode, "nul") == 0)
    {
        static const char bytes[] = {'a', 0, 'b', '\n'};
        fwrite(bytes, 1, sizeof bytes, stdout);
    }
    char bytes[1024];
    size_t count;
    while ((count = fread(bytes, 1U, sizeof bytes, stdin)) != 0U)
    {
        if (fwrite(bytes, 1U, count, stdout) != count)
            return 3;
        fflush(stdout);
    }
    if (ferror(stdin))
        return 4;
    fputs("\nEOF\n", stdout);
    fputs("finished input\n", stderr);
    return 0;
}
