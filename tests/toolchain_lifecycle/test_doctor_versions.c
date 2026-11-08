/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_doctor_versions.c
 *
 * PURPOSE:
 *   Verify version-only CLI diagnostics cannot enter compile/link/run preparation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../tools/umicom/src/cli.h"
#include "umicom/toolchain/version_inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned discovery_calls, full_prepare_calls;
static UmiStatus discovery_result = UMI_STATUS_OK;
static int request_valid;
static unsigned save_calls;
static UmiStatus save_result = UMI_STATUS_OK;
static int save_valid;
#ifdef _WIN32
static const char report_file[] = "C:/fixture/tools.json";
#else
static const char report_file[] = "/fixture/tools.json";
#endif
static UmiStatus FakeSave(const UmiToolchainDiscoveryReport *report, size_t size,
    UmiStatus result, const char *destination)
{
    ++save_calls;
    save_valid = discovery_calls == 1U && size == sizeof(*report) &&
        result == discovery_result && !strcmp(destination, report_file) &&
        report->profile.tools[UMI_TOOL_GCC].state == UMI_TOOL_VALIDATED;
    return save_result;
}
/* Replace only the existing discovery boundary. The test never executes a
 * compiler, version command, package probe or generated process. */
static UmiStatus FakeDiscovery(const UmiToolchainDiscoveryRequest *request,
    UmiToolchainDiscoveryReport *report)
{
    ++discovery_calls;
    request_valid = request != NULL && request->skip_compile_probe == 1 &&
        !request->require_gtk && !request->require_github_cli && !request->require_java;
    memset(report, 0, sizeof(*report));
    umi_toolchain_profile_init(&report->profile);
    UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(&report->profile, UMI_TOOL_GCC);
    if (tool != NULL) {
        tool->state = UMI_TOOL_VALIDATED;
        tool->required = 1;
        (void)snprintf(tool->path, sizeof(tool->path), "C:/fixture tools/gcc.exe");
        (void)snprintf(tool->version, sizeof(tool->version), "fixture-version");
    }
    return discovery_result;
}
UmiStatus umi_cli_context_prepare(UmiCliContext *context, const char *root, int gtk, int github)
{
    (void)context; (void)root; (void)gtk; (void)github;
    ++full_prepare_calls;
    return UMI_STATUS_INVALID_STATE;
}
void umi_cli_diagnostic_sink(const UmiDiagnostic *diagnostic, void *context)
{ (void)diagnostic; (void)context; }

#define umi_toolchain_discover FakeDiscovery
#define umi_toolchain_version_inventory_save_sized FakeSave
#include "../../tools/umicom/src/command_check.c"
#undef umi_toolchain_discover
#undef umi_toolchain_version_inventory_save_sized

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiCliContext *context = calloc(1U, sizeof(*context));
    if (context == NULL) return 1;
    /* Leave room for malformed duplicate/report combinations. The former
     * two-slot vector is retained to show the initial console-only contract. */
#if 0
    char *options[] = {"--versions-only", NULL};
#endif
    char *options[7] = {"--versions-only", NULL};
    int count = 1, expected = 2;
    unsigned expected_calls = 0U, expected_saves = 0U;
    char **arguments = options;
    const char *mode = argv[1];
    if (!strcmp(mode, "versions") || !strcmp(mode, "missing")) {
        discovery_result = !strcmp(mode, "missing") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
        expected = discovery_result == UMI_STATUS_OK ? 0 : 1;
        expected_calls = 1U;
    } else if (!strcmp(mode, "report") || !strcmp(mode, "report-incomplete") ||
        !strcmp(mode, "report-failure") || !strcmp(mode, "report-first")) {
        options[1] = "--report"; options[2] = (char *)report_file; count = 3;
        expected_calls = 1U; expected_saves = 1U; expected = 0;
        if (!strcmp(mode, "report-incomplete")) { discovery_result = UMI_STATUS_NOT_FOUND; expected = 1; }
        if (!strcmp(mode, "report-failure")) { save_result = UMI_STATUS_IO_ERROR; expected = 2; }
        if (!strcmp(mode, "report-first")) { options[0] = "--report"; options[1] = (char *)report_file; options[2] = "--versions-only"; }
    } else if (!strcmp(mode, "report-alone")) { options[0] = "--report"; options[1] = (char *)report_file; count = 2; }
    else if (!strcmp(mode, "report-missing")) { options[1] = "--report"; count = 2; }
    else if (!strcmp(mode, "report-relative")) { options[1] = "--report"; options[2] = "relative.json"; count = 3; }
    else if (!strcmp(mode, "report-option")) { options[1] = "--report"; options[2] = "--all"; count = 3; }
    else if (!strcmp(mode, "report-null")) { options[1] = "--report"; count = 3; }
    else if (!strcmp(mode, "report-duplicate")) { options[1] = "--report"; options[2] = (char *)report_file; options[3] = options[1]; options[4] = options[2]; count = 5; }
    else if (!strcmp(mode, "report-mixed")) { options[1] = "--report"; options[2] = (char *)report_file; options[3] = "--all"; count = 4; }
    else if (!strcmp(mode, "duplicate")) { options[1] = options[0]; count = 2; }
    else if (!strcmp(mode, "mixed")) { options[1] = "--all"; count = 2; }
    else if (!strcmp(mode, "null-vector")) arguments = NULL;
    else if (!strcmp(mode, "null-value")) options[0] = NULL;
    else if (!strcmp(mode, "empty-value")) options[0] = "";
    else if (!strcmp(mode, "negative-argc")) count = -1;
    else if (!strcmp(mode, "help")) { options[0] = "--help"; expected = 0; }
    else if (!strcmp(mode, "null-context")) { free(context); context = NULL; }
    else { free(context); return 2; }
    int result = umi_cli_command_check(context, count, arguments);
    int failed = result != expected || full_prepare_calls != 0U ||
        discovery_calls != expected_calls || (expected_calls != 0U && !request_valid) ||
        save_calls != expected_saves || (expected_saves != 0U && !save_valid);
    /* A successful version inventory must not synthesize probe success or an
     * execution environment that subsequent commands can mistake for readiness. */
    if (context != NULL && (context->environment_ready || context->discovery.compile_probe_passed ||
        context->discovery.link_probe_passed || context->discovery.runtime_probe_passed ||
        context->discovery.c23_probe_passed)) failed = 1;
    if (failed) fprintf(stderr, "mode=%s result=%d expected=%d discovery=%u full=%u request=%d\n",
        mode, result, expected, discovery_calls, full_prepare_calls, request_valid);
    free(context);
    return failed;
}
