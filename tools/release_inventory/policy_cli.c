/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tools/release_inventory/policy_cli.c
 * Purpose: Explicit file/console commands for Framework's test policy service.
 *---------------------------------------------------------------------------*/
#include "policy_cli.h"
#include "umicom/distribution/runtime/test_policy.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static wchar_t *Wide(const char *path)
{
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (n <= 0) return NULL;
    wchar_t *wide = calloc((size_t)n, sizeof(*wide));
    if (wide != NULL && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, n) != n) {
        free(wide); return NULL;
    }
    return wide;
}
#endif
static UmiStatus ReadPolicy(const char *path, UmiReleaseTestPolicy **out)
{
    FILE *file;
#ifdef _WIN32
    wchar_t *wide = Wide(path);
    if (wide == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = _wfopen(wide, L"rb"); free(wide);
#else
    file = fopen(path, "rb");
#endif
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    char *text = malloc(UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U);
    if (text == NULL) { (void)fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t size = fread(text, 1U, UMI_RELEASE_INVENTORY_TEXT_LIMIT + 1U, file);
    bool failed = ferror(file) != 0;
    if (fclose(file) != 0) failed = true;
    UmiStatus status = failed ? UMI_STATUS_IO_ERROR : size > UMI_RELEASE_INVENTORY_TEXT_LIMIT ?
        UMI_STATUS_CAPACITY_EXCEEDED : UmiReleaseTestPolicyParse(text, size, out);
    free(text); return status;
}
/* Create a new selected output only. Retain every previous decision document;
 * never truncate a file or follow an existing output symlink. Parent directories
 * must be trusted. A failed write may leave a partial new file for inspection. */
static UmiStatus WriteNew(const char *path, const char *text, size_t length)
{
#ifdef _WIN32
    wchar_t *wide = Wide(path);
    if (wide == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE file = CreateFileW(wide, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD openError = file == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS;
    free(wide);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = openError;
        return error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    }
    size_t offset = 0U; bool failed = false;
    while (offset < length) {
        DWORD written = 0;
        DWORD chunk = (DWORD)(length - offset); /* Native output is bounded to 64 MiB. */
        if (!WriteFile(file, text + offset, chunk, &written, NULL) || written == 0U) { failed = true; break; }
        offset += (size_t)written;
    }
    if (!FlushFileBuffers(file)) failed = true;
    if (!CloseHandle(file)) failed = true;
#else
    FILE *file = fopen(path, "wbx");
    if (file == NULL) return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    bool failed = fwrite(text, 1U, length, file) != length;
    if (fclose(file) != 0) failed = true;
#endif
    return failed ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static const char *Requirement(UmiReleaseTestRequirement requirement)
{
    return requirement == UMI_RELEASE_TEST_REQUIRED ? "required" :
        requirement == UMI_RELEASE_TEST_OPTIONAL ? "optional" : "unassigned";
}
typedef struct Printer { void (*text)(const char *); } Printer;
static void Finding(void *context, UmiReleaseTestPolicyFinding finding, const char *name)
{
    static const char *const reasons[] = { "missing-rule", "orphan-rule", "unassigned",
        "required-unavailable", "optional-unavailable", "missing-registration", "added-registration" };
    const Printer *printer = context;
    (void)printf("%s: ", reasons[finding]); printer->text(name); (void)putchar('\n');
}
int UmiReleaseInventoryPolicyCommand(int argc, char **argv,
    UmiStatus (*readInventory)(const char *, UmiReleaseInventory **), void (*printText)(const char *))
{
    bool draft = argc == 4 && strcmp(argv[1], "draft-policy") == 0;
    bool review = argc == 3 && strcmp(argv[1], "review-policy") == 0;
    bool edit = argc == 8 && strcmp(argv[1], "set-policy") == 0;
    bool assess = argc == 5 && strcmp(argv[1], "assess-policy") == 0;
    if (!draft && !review && !edit && !assess) { (void)fputs("Use --help for policy command arguments.\n", stderr); return 2; }
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    UmiReleaseTestPolicy *policy = NULL;
    char *text = NULL; size_t length = 0U;
    UmiStatus status = UMI_STATUS_OK; int result = 2;
    if (draft || assess) status = readInventory(argv[2], &configured);
    if (status == UMI_STATUS_OK && assess) status = readInventory(argv[3], &observed);
    if (status == UMI_STATUS_OK && !draft) status = ReadPolicy(assess ? argv[4] : argv[2], &policy);
    if (status == UMI_STATUS_OK && draft) {
        status = UmiReleaseTestPolicyDraft(configured, &text, &length);
        if (status == UMI_STATUS_OK) status = WriteNew(argv[3], text, length);
        if (status == UMI_STATUS_OK) { (void)puts("Created an unassigned draft. Every test still needs a reviewed decision."); result = 0; }
    } else if (status == UMI_STATUS_OK && edit) {
        UmiReleaseTestRequirement requirement = UMI_RELEASE_TEST_UNASSIGNED;
        if (strcmp(argv[4], "required") == 0) requirement = UMI_RELEASE_TEST_REQUIRED;
        else if (strcmp(argv[4], "optional") == 0) requirement = UMI_RELEASE_TEST_OPTIONAL;
        else if (strcmp(argv[4], "unassigned") != 0) status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK) status = UmiReleaseTestPolicyEdit(policy, argv[3], requirement, argv[5], argv[6], &text, &length);
        if (status == UMI_STATUS_OK) status = WriteNew(argv[7], text, length);
        if (status == UMI_STATUS_OK) { (void)puts("Created a new policy revision; the input document was retained."); result = 0; }
    } else if (status == UMI_STATUS_OK && review) {
        for (size_t i = 0U; i < UmiReleaseTestPolicyCount(policy); ++i) {
            const UmiReleaseTestRule *rule = UmiReleaseTestPolicyAt(policy, i);
            printText(rule->name); (void)printf("\t%s\t", Requirement(rule->requirement));
            printText(rule->owner); (void)putchar('\t'); printText(rule->reason); (void)putchar('\n');
        }
        result = 0;
    } else if (status == UMI_STATUS_OK && assess) {
        UmiReleaseTestPolicyAssessment assessment = {0}; Printer printer = { printText };
        status = UmiReleaseTestPolicyAssess(configured, observed, policy, Finding, &printer, &assessment);
        if (status == UMI_STATUS_OK) {
            (void)printf("Tests: %zu; rules: %zu; required: %zu; optional: %zu; unassigned: %zu.\n"
                "Missing rules: %zu; orphan rules: %zu; required unavailable: %zu; optional unavailable: %zu.\n"
                "Missing registrations: %zu; added registrations: %zu.\n", assessment.expectedTests,
                assessment.policyRules, assessment.requiredTests, assessment.optionalTests, assessment.unassignedTests,
                assessment.missingRules, assessment.orphanRules, assessment.requiredUnavailable, assessment.optionalUnavailable,
                assessment.missingRegistrations, assessment.addedRegistrations);
            (void)puts(assessment.readyForExecution ? "REGISTRATION POLICY READY FOR EXECUTION. Test results still required." :
                "REGISTRATION POLICY BLOCKED. Review the findings; no release is approved.");
            result = assessment.readyForExecution ? 0 : 1;
        }
    }
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "Policy command refused: %s. Existing files were not overwritten.\n", umi_status_text(status));
    if (fflush(stdout) != 0 || ferror(stdout) != 0) result = 2;
    UmiReleaseTestPolicyTextDestroy(text); UmiReleaseTestPolicyDestroy(policy);
    UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return result;
}
