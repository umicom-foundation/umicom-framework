/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/tool_probe.c
 *
 * PURPOSE:
 *   Implement one-tool discovery from an explicit root or PATH using argument-safe execution.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/tool_probe.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/discovery.h"

/* Provide the find from root operation used by this module and its client applications. */
static UmiStatus find_from_root(const char *root, UmiToolKind kind,
                                char *out_path, size_t capacity)
{
    char bin[UMI_TOOL_PATH_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0') return UMI_STATUS_NOT_FOUND;
    status = umi_fs_join(bin, sizeof(bin), root, "bin");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_join(out_path, capacity, bin, umi_tool_default_executable(kind));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_fs_is_file(out_path) ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/* Provide the first line operation used by this module and its client applications. */
static void first_line(char *text)
{
    char *end;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;
    end = strpbrk(text, "\r\n");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end != NULL) *end = '\0';
}

/*
 * Provide the toolchain tool probe operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_tool_probe(const UmiToolchainToolProbeRequest *request,
                                   UmiToolInfo *out_tool,
                                   UmiToolchainProbeReport *out_report)
{
    const char *arguments[] = {"--version"};
    int exit_code = -1;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_tool == NULL || out_report == NULL ||
        request->kind < 0 || request->kind >= UMI_TOOL_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_tool, 0, sizeof(*out_tool));
    out_tool->kind = request->kind;
    out_tool->state = UMI_TOOL_MISSING;
    umi_toolchain_probe_report_init(out_report, request->kind);

    status = find_from_root(request->explicit_root, request->kind,
                            out_tool->path, sizeof(out_tool->path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) out_report->from_explicit_root = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        status = umi_toolchain_find_on_path(
            umi_tool_default_executable(request->kind),
            out_tool->path, sizeof(out_tool->path));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_report->status = UMI_STATUS_NOT_FOUND;
        (void)snprintf(out_report->detail, sizeof(out_report->detail),
                       "%s was not found for this operation.",
                       umi_tool_kind_name(request->kind));
        return UMI_STATUS_NOT_FOUND;
    }

    out_tool->state = UMI_TOOL_FOUND;
    out_report->found = 1;
    (void)snprintf(out_report->path, sizeof(out_report->path), "%s", out_tool->path);
    /* Apply this operation only while the related capability or state is available. */
    if (!request->validate_version) {
        out_tool->state = UMI_TOOL_VALIDATED;
        out_report->validated = 1;
        out_report->status = UMI_STATUS_OK;
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->kind == UMI_TOOL_MSVC_CL || request->kind == UMI_TOOL_MSVC_LINK) {
        arguments[0] = "/?";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (request->kind == UMI_TOOL_VSWHERE) {
        arguments[0] = "-help";
    }
    status = umi_process_capture(out_tool->path, arguments, 1U,
                                 out_tool->version, sizeof(out_tool->version),
                                 &exit_code);
    first_line(out_tool->version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        (exit_code == 0 ||
         ((request->kind == UMI_TOOL_MSVC_CL || request->kind == UMI_TOOL_MSVC_LINK) &&
          out_tool->version[0] != '\0'))) {
        out_tool->state = UMI_TOOL_VALIDATED;
        out_report->validated = 1;
        out_report->status = UMI_STATUS_OK;
        (void)snprintf(out_report->version, sizeof(out_report->version), "%s", out_tool->version);
        return UMI_STATUS_OK;
    }

    out_tool->state = UMI_TOOL_FAILED;
    out_report->status = status == UMI_STATUS_OK ? UMI_STATUS_UNAVAILABLE : status;
    (void)snprintf(out_report->detail, sizeof(out_report->detail),
                   "%s was found but could not be validated (exit %d).",
                   umi_tool_kind_name(request->kind), exit_code);
    return out_report->status;
}
