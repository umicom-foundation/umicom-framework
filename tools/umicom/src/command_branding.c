/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_branding.c
 *
 * PURPOSE:
 *   Expose read-only canonical guide identity checks through the native Umicom CLI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int BrandHelp(int code)
{
    puts("Usage: umicom branding audit --assets BRAND_DIRECTORY --root GUIDES [--root GUIDES ...]\n"
         "Compare standalone HTML guide markers, font roles and decoded SVG/ICO bytes.\n"
         "Select Framework resources/brand and the guide directories explicitly.\n"
         "Read-only: no build, browser, font installation, source rewrite or publication.\n"
         "Exit 0: source contract matches; 1: branding findings; 2: input/IO/structure failure.\n"
         "A source match does not qualify rendered layout, contrast or installed fonts.");
    return code;
}
static void BrandDiagnostic(void *context, const char *path, size_t line,
    const char *rule, const char *message)
{
    (void)context;
    fprintf(stderr, "%s:%zu [%s] %s\n", path, line, rule, message);
}
int umi_cli_command_branding(int argc, char **argv)
{
    /* Admit the complete vector before help/option lookups. The CLI only
     * presents results; Framework owns traversal, parsing and asset identity. */
    if (argc < 0 || argc > 1024 || (argc != 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) return BrandHelp(0);
    if (argc < 1 || strcmp(argv[0], "audit") != 0) return BrandHelp(2);
    if (argc == 2 && strcmp(argv[1], "--help") == 0) return BrandHelp(0);
    const char *assets = NULL;
    const char **roots = calloc((size_t)argc, sizeof(*roots));
    if (roots == NULL) return 2;
    size_t count = 0U;
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc || strncmp(argv[i + 1], "--", 2U) == 0) { free(roots); return BrandHelp(2); }
        if (strcmp(argv[i], "--assets") == 0 && assets == NULL) assets = argv[i + 1];
        else if (strcmp(argv[i], "--root") == 0) roots[count++] = argv[i + 1];
        else { free(roots); return BrandHelp(2); }
    }
    if (assets == NULL || count == 0U) { free(roots); return BrandHelp(2); }
    UmiSourceContractReport report = {0}; report.diagnostic = BrandDiagnostic;
    int result = UmiSourceContractAuditGuides(roots, count, assets, &report);
    free(roots);
    printf("Guide branding: %zu source reads, %zu checks, %zu findings, %zu input errors.\n",
        report.files_checked, report.checks, report.findings, report.input_errors);
    return report.input_errors != 0U ? 2 : result;
}
