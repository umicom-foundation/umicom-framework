/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/source_contracts/main.c
 *
 * PURPOSE:
 *   Run Framework-owned read-only qualification without a scripting interpreter.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Terminal diagnostics intentionally contain no source bodies or credentials. */
static void diagnostic(void *context, const char *path, size_t line,
    const char *rule, const char *message)
{
    (void)context;
    (void)fprintf(stderr, "[FAIL] %s:%zu [%s] %s\n", path, line, rule, message);
}
static int usage(void)
{
    (void)fprintf(stderr,
        "Umicom native source contracts\n"
        "  umicom-source-contracts headers --root INCLUDE [--root INCLUDE ...]\n"
        "  umicom-source-contracts comments --root SOURCE [--root SOURCE ...]\n"
        "  umicom-source-contracts workbench --source-dir APPLICATIONS [--check GROUP]\n"
        "Read-only qualification. Exit 0: pass; 1: contract findings; 2: input failure.\n"
        "Source wiring does not prove rendered/native application behaviour.\n");
    return 2;
}
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0) { (void)usage(); return 0; }
    if (argc < 4) return usage();
    UmiSourceContractReport report = {0}; report.diagnostic = diagnostic;
    int result;
    if (strcmp(argv[1], "workbench") == 0) {
        if ((argc != 4 && argc != 6) || strcmp(argv[2], "--source-dir") != 0) return usage();
        if (argc == 6 && strcmp(argv[4], "--check") != 0) return usage();
        result = UmiSourceContractWorkbenchGroup(argv[3], argc == 6 ? argv[5] : NULL, &report);
    } else {
        bool headers = strcmp(argv[1], "headers") == 0;
        if (!headers && strcmp(argv[1], "comments") != 0) return usage();
        const char **roots = calloc((size_t)argc, sizeof(*roots));
        if (roots == NULL) return 2;
        size_t count = 0U;
        for (int i = 2; i < argc; i += 2) {
            if (i + 1 >= argc || strcmp(argv[i], "--root") != 0 || argv[i + 1][0] == '\0') { free(roots); return usage(); }
            roots[count++] = argv[i + 1];
        }
        result = UmiSourceContractAudit(roots, count, headers, &report); free(roots);
    }
    (void)printf("%s: %zu source reads, %zu checks, %zu findings, %zu input errors.\n",
        argv[1], report.files_checked, report.checks, report.findings, report.input_errors);
    if (report.input_errors != 0U) return 2;
    return result;
}
