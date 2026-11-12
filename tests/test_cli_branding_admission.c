/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_cli_branding_admission.c
 *
 * PURPOSE:
 *   Verify native branding option refusal before filesystem traversal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "cli.h"
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char *help[] = {"--help"};
    char *nested_help[] = {"audit", "--help"};
    char *bad[] = {"audit", "--assets", "not-opened", "--root", "not-opened"};
    int expected = 2, count = 0;
    char **args = bad;
    if (strcmp(argv[1], "help") == 0) { args = help; count = 1; expected = 0; }
    else if (strcmp(argv[1], "nested-help") == 0) { args = nested_help; count = 2; expected = 0; }
    else if (strcmp(argv[1], "empty") == 0) count = 0;
    else if (strcmp(argv[1], "negative") == 0) count = -1;
    else if (strcmp(argv[1], "too-many") == 0) count = 1025;
    else if (strcmp(argv[1], "null-vector") == 0) { count = 1; args = NULL; }
    else if (strcmp(argv[1], "null-item") == 0) { count = 5; bad[4] = NULL; }
    else if (strcmp(argv[1], "empty-item") == 0) { count = 5; bad[4] = ""; }
    else if (strcmp(argv[1], "missing-value") == 0) count = 4;
    else if (strcmp(argv[1], "missing-root") == 0) count = 3;
    else if (strcmp(argv[1], "missing-assets") == 0) { count = 3; bad[1] = "--root"; }
    else if (strcmp(argv[1], "duplicate-assets") == 0) { count = 5; bad[3] = "--assets"; }
    else if (strcmp(argv[1], "unknown-option") == 0) { count = 5; bad[3] = "--unknown"; }
    else if (strcmp(argv[1], "option-value") == 0) { count = 5; bad[2] = "--root"; }
    else if (strcmp(argv[1], "unknown-command") == 0) { count = 5; bad[0] = "rewrite"; }
    else return 2;
    assert(umi_cli_command_branding(count, args) == expected);
    return 0;
}
