/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/release_inventory/main.c
 * PURPOSE:
 *   Thin file/console boundary for the distribution inventory service.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tools/release_inventory/main.c
 * Purpose: Thin file/console boundary for the distribution inventory service.
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/inventory.h"
#include "policy_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#endif

/* Keep all reusable parsing and comparison in Framework. This adapter only
 * reads explicitly selected ordinary files; it never launches or writes them.
 * As with other local tooling, concurrent file replacement is not locked out. */
static UmiStatus Read(const char *path, UmiReleaseInventory **out)
{
    FILE *file = NULL;
#ifdef _WIN32
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (n <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t *wide = calloc((size_t)n, sizeof(*wide));
    if (wide == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, n) == n)
        file = _wfopen(wide, L"rb");
    free(wide);
#else
    file = fopen(path, "rb");
#endif
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    char *text = malloc(UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U);
    if (text == NULL) { (void)fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t size = fread(text, 1U, UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U, file);
    bool failed = ferror(file) != 0;
    if (fclose(file) != 0) failed = true;
    UmiStatus status = failed ? UMI_STATUS_IO_ERROR :
        size > UMI_RELEASE_INVENTORY_TEXT_LIMIT ? UMI_STATUS_CAPACITY_EXCEEDED :
        UmiReleaseInventoryParse(text, size, out);
    free(text);
    return status;
}

static void PrintText(const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < 32U || *p == 127U || *p == '\\') (void)printf("\\x%02x", (unsigned)*p);
        else (void)putchar((int)*p);
    }
}
static void Difference(void *context, UmiReleaseInventoryDifference difference, const char *name)
{
    (void)context;
    (void)fputs(difference == UMI_RELEASE_INVENTORY_TEST_MISSING ? "missing: " : "added: ", stdout);
    PrintText(name); (void)putchar('\n');
}
static int Run(int argc, char **argv)
{
    /* Framework owns decisions and assessment; this adapter delegates the new
     * explicit policy commands while retaining the original inspection paths. */
    if (argc > 1 && (strcmp(argv[1], "draft-policy") == 0 || strcmp(argv[1], "review-policy") == 0 ||
        strcmp(argv[1], "set-policy") == 0 || strcmp(argv[1], "assess-policy") == 0))
        return UmiReleaseInventoryPolicyCommand(argc, argv, Read, PrintText);
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        (void)puts("Test policy commands:\n  draft-policy CONFIGURED.tsv NEW.tsv\n  review-policy POLICY.tsv\n"
            "  set-policy POLICY.tsv EXACT_NAME required|optional|unassigned OWNER REASON NEW.tsv\n"
            "  assess-policy CONFIGURED.tsv CTEST.tsv POLICY.tsv\n"
            "Draft/set create only a new explicitly selected file; existing outputs are refused.\n"
            "Policy exit 0: command complete or registration ready; 1: policy blocked; 2: input/I/O error.");
        (void)puts("Umicom release inventory\n  show INVENTORY.tsv\n  list INVENTORY.tsv\n"
            "  compare CONFIGURED.tsv CTEST.tsv\n"
            "Exit 0: inspection complete or exact names match with commands and no disabled tests;\n"
            "1: names differ, disabled tests or missing commands; 2: invalid input/context or I/O.\n"
            "A match is registration evidence only: no tests are run and no release is approved.");
        return fflush(stdout) == 0 && ferror(stdout) == 0 ? 0 : 2;
    }
    bool show = argc == 3 && strcmp(argv[1], "show") == 0;
    bool list = argc == 3 && strcmp(argv[1], "list") == 0;
    bool compare = argc == 4 && strcmp(argv[1], "compare") == 0;
    if (!show && !list && !compare) { (void)fputs("Use --help for arguments.\n", stderr); return 2; }
    UmiReleaseInventory *left = NULL, *right = NULL;
    UmiStatus status = Read(argv[2], &left);
    if (status == UMI_STATUS_OK && compare) status = Read(argv[3], &right);
    int result = 2;
    if (status == UMI_STATUS_OK && compare) {
        UmiReleaseInventoryComparison comparison = {0};
        status = UmiReleaseInventoryCompareTests(left, right, Difference, NULL, &comparison);
        if (status == UMI_STATUS_OK) {
            (void)printf("Configured: %zu; CTest: %zu; missing: %zu; added: %zu; disabled: %zu; missing commands: %zu.\n",
                comparison.expectedTests, comparison.observedTests, comparison.missingTests,
                comparison.addedTests, comparison.disabledTests, comparison.missingCommands);
            result = comparison.namesMatch && comparison.disabledTests == 0U && comparison.missingCommands == 0U ? 0 : 1;
        }
    } else if (status == UMI_STATUS_OK) {
        UmiReleaseInventorySummary summary = {0};
        status = UmiReleaseInventorySummarise(left, &summary);
        (void)fputs("Producer: ", stdout); PrintText(UmiReleaseInventoryProducer(left));
        (void)fputs("; configuration: ", stdout); PrintText(UmiReleaseInventoryConfiguration(left));
        (void)fputs("\nSource: ", stdout); PrintText(UmiReleaseInventorySourceRoot(left));
        (void)fputs("\nBuild: ", stdout); PrintText(UmiReleaseInventoryBuildRoot(left));
        (void)printf("\nHeaders: %zu (unassigned: %zu); targets: %zu; source/target pairs: %zu (unresolved: %zu).\n"
            "Tests: %zu; disabled: %zu; missing commands: %zu.\n", summary.headers,
            summary.unassignedHeaders, summary.targets, summary.sources, summary.unresolvedSources,
            summary.tests, summary.disabledTests, summary.missingCommands);
        if (list) {
            static const char *const kinds[] = { "header", "target", "source", "test" };
            for (size_t i = 0U; i < UmiReleaseInventoryCount(left); ++i) {
                const UmiReleaseInventoryRecord *row = UmiReleaseInventoryAt(left, i);
                (void)printf("%s\t", kinds[row->kind]); PrintText(row->identity);
                (void)putchar('\t'); PrintText(row->owner);
                (void)printf("\t%s\t", row->state); PrintText(row->detail); (void)putchar('\n');
            }
        }
        result = 0;
    }
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "Inventory refused: %s.\n", umi_status_text(status));
    else (void)puts("REGISTRATION ONLY. Review unassigned ownership and run the required tests separately.");
    if (fflush(stdout) != 0 || ferror(stdout) != 0) result = 2;
    UmiReleaseInventoryDestroy(right); UmiReleaseInventoryDestroy(left);
    return result;
}
#ifdef _WIN32
int wmain(int argc, wchar_t **argv)
{
    char **args = calloc((size_t)argc + 1U, sizeof(*args));
    if (args == NULL) return 2;
    int result = 2; bool valid = true;
    for (int i = 0; i < argc; ++i) {
        int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, argv[i], -1, NULL, 0, NULL, NULL);
        if (n <= 0 || (args[i] = malloc((size_t)n)) == NULL ||
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, argv[i], -1, args[i], n, NULL, NULL) != n) {
            valid = false; break;
        }
    }
    if (valid) result = Run(argc, args);
    for (int i = 0; i < argc; ++i) free(args[i]);
    free(args); return result;
}
#else
int main(int argc, char **argv) { return Run(argc, argv); }
#endif
