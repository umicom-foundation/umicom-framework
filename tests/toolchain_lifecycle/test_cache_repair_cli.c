/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_cache_repair_cli.c
 *
 * PURPOSE:
 *   Check CLI repair path admission and preserve recovery evidence before later failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../../tools/umicom/src/cli.h"
#include "umicom/toolchain/user_presets.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
static unsigned joins, repairs, writes;
static char captured[16384];
static size_t captured_size;
static int FixturePrint(const char *format, ...)
{
    va_list arguments; va_start(arguments, format);
    int count = vsnprintf(captured + captured_size, sizeof captured - captured_size, format, arguments);
    va_end(arguments);
    if (count < 0 || (size_t)count >= sizeof captured - captured_size) return -1;
    captured_size += (size_t)count;
    return count;
}
UmiStatus umi_cli_context_prepare(UmiCliContext *context, const char *root, int gtk, int github)
{
    (void)root; (void)gtk; (void)github; memset(context, 0, sizeof *context);
#ifdef _WIN32
    strcpy(context->project_root, "C:/fixture");
#else
    strcpy(context->project_root, "/fixture");
#endif
    return UMI_STATUS_OK;
}
UmiStatus umi_cli_context_prepare_operation(UmiCliContext *context, const char *root, UmiToolchainOperationKind operation)
{ (void)context; (void)root; (void)operation; return UMI_STATUS_UNAVAILABLE; }
static UmiStatus FixtureJoin(char *out, size_t capacity, const char *left, const char *right)
{
    ++joins;
    if ((!strcmp(mode, "first-join") && joins == 1U) || (!strcmp(mode, "second-join") && joins == 2U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_path_join(left, right, out, capacity);
}
static UmiStatus FixtureRepair(const UmiToolchainProfile *profile, const char *directory,
    char *out, size_t capacity, int dry_run)
{
    (void)profile; (void)directory; (void)capacity; (void)dry_run; ++repairs;
    if (!strcmp(mode, "healthy")) return UMI_STATUS_OK; /* Verify CLI initialization too. */
    if (!strcmp(mode, "partial-rename")) { strcpy(out, "recovery-completed"); return UMI_STATUS_IO_ERROR; }
    if (!strcmp(mode, "repair-failure")) return UMI_STATUS_PARSE_ERROR;
    strcpy(out, "recovery-proposed"); return UMI_STATUS_OK;
}
/* The old fixture modeled replacement only. Preserve it while checking the
 * explicit preview/apply creation contract used by the CLI now. */
#if 0
static UmiStatus FixturePresets(const UmiToolchainProfile *profile, const char *root, const char *path)
{
    (void)profile; (void)root; (void)path; ++writes;
    return !strcmp(mode, "preset-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
#endif
/* Extend the preview/apply fixture to distinguish preflight refusal from a
 * racing create. Retain its initial behavior for review. */
#if 0
static UmiStatus FixturePresets(const UmiToolchainProfile *profile, const char *root, const char *path, int dry_run)
{
    (void)profile; (void)root; (void)path;
    if (dry_run) return UMI_STATUS_OK;
    ++writes;
    return !strcmp(mode, "preset-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
#endif
static UmiStatus FixturePresets(const UmiToolchainProfile *profile, const char *root, const char *path, int dry_run)
{
    (void)profile; (void)root; (void)path;
    if (dry_run) {
        if (!strcmp(mode, "preset-existing")) return UMI_STATUS_ALREADY_EXISTS;
        if (!strcmp(mode, "preset-invalid")) return UMI_STATUS_INVALID_ARGUMENT;
        return UMI_STATUS_OK;
    }
    ++writes;
    if (!strcmp(mode, "preset-race")) return UMI_STATUS_ALREADY_EXISTS;
    return !strcmp(mode, "preset-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
#define printf FixturePrint
#define umi_fs_join FixtureJoin
#define umi_build_repair_cache FixtureRepair
#define UmiBuildUserPresetsCreate FixturePresets
#include "../../tools/umicom/src/command_build.c"
#undef UmiBuildUserPresetsCreate
#undef umi_build_repair_cache
#undef umi_fs_join
#undef printf
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    if (strcmp(mode, "healthy") && strcmp(mode, "preview") && strcmp(mode, "first-join") && strcmp(mode, "second-join") && strcmp(mode, "repair-failure") && strcmp(mode, "partial-rename") && strcmp(mode, "preset-failure") && strcmp(mode, "preset-existing") && strcmp(mode, "preset-invalid") && strcmp(mode, "preset-race")) return 2;
    static UmiCliContext context;
    char *args[] = {"--dry-run"};
    int preview = !strcmp(mode, "preview");
    int result = umi_cli_command_repair(&context, preview ? 1 : 0, preview ? args : NULL);
    unsigned expected_repairs = strcmp(mode, "first-join") && strcmp(mode, "second-join") && strcmp(mode, "preset-existing") && strcmp(mode, "preset-invalid") ? 1U : 0U;
    unsigned expected_writes = !strcmp(mode, "healthy") || !strcmp(mode, "preset-failure") || !strcmp(mode, "preset-race") ? 1U : 0U;
    int expected = !strcmp(mode, "healthy") || preview ? 0 : 1;
    if (result != expected || repairs != expected_repairs || writes != expected_writes ||
        joins != (!strcmp(mode, "first-join") ? 1U : 2U)) {
        fprintf(stderr, "result=%d joins=%u repairs=%u writes=%u\n", result, joins, repairs, writes);
        return 1;
    }
    /* Check captured evidence in the program itself so a CTest output regex
     * cannot mask an assertion failure through its exit-code precedence. */
    if (!strcmp(mode, "partial-rename") &&
        strstr(captured, "Recorded cache recovery path: recovery-completed") == NULL) return 1;
    if (!strcmp(mode, "healthy") && strstr(captured, "cache recovery path:") != NULL) return 1;
    return 0;
}
