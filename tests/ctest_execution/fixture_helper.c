/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/fixture_helper.c
 *
 * PURPOSE:
 *   Supply predictable native test bodies for CTest integration checks.
 *   Any files written by this helper are inside a disposable fixture build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2) return 2;
    if (strcmp(argv[1], "pass") == 0) return 0;
    if (strcmp(argv[1], "fail") == 0) { puts("Umicom deliberately failing fixture."); return 5; }
    if (strcmp(argv[1], "skip") == 0) return 77;
    if (strcmp(argv[1], "regex-skip") == 0) { puts("UMICOM_SKIP_THIS_FIXTURE"); return 0; }
    if (strcmp(argv[1], "cwd") == 0) {
        FILE *file = fopen("umicom-working-directory.marker", "rb");
        if (file == NULL) return 3;
        return fclose(file) == 0 ? 0 : 4;
    }
    if (strcmp(argv[1], "large") == 0) {
        for (unsigned index = 0U; index < 12000U; ++index)
            puts("Umicom test output: deterministic text to exercise bounded capture and result-file reading.");
        puts("UMICOM_TAIL_MARKER");
        return 5;
    }
    if (argc != 3) return 2;
    if (strcmp(argv[1], "require") == 0) {
        FILE *file = fopen(argv[2], "rb");
        if (file == NULL) return 6;
        return fclose(file) == 0 ? 0 : 4;
    }
    if (strcmp(argv[1], "remove") == 0) return remove(argv[2]) == 0 ? 0 : 6;
    if (strcmp(argv[1], "create") == 0 || strcmp(argv[1], "sentinel") == 0) {
        FILE *file = fopen(argv[2], "wb");
        if (file == NULL) return 6;
        int wrote = fputs("Umicom test-owned fixture marker.\n", file) >= 0;
        int closed = fclose(file) == 0;
        if (!wrote || !closed) return 6;
        return strcmp(argv[1], "sentinel") == 0 ? 7 : 0;
    }
    return 2;
}
