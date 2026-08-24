/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/scoped_discovery.c
 *
 * PURPOSE:
 *   Implement operation-scoped discovery that never requires unrelated build tools.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/scoped_discovery.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/toolchain/discovery.h"

static UmiStatus probe_into_profile(
    UmiToolchainProfile *profile,
    const UmiToolchainRequirement *requirement,
    const UmiToolchainScopedDiscoveryRequest *request,
    size_t *found)
{
    UmiToolchainToolProbeRequest probe;
    UmiToolchainProbeReport report;
    UmiToolInfo tool;
    UmiToolInfo *slot;
    UmiStatus status;

    (void)memset(&probe, 0, sizeof(probe));
    probe.kind = requirement->kind;
    probe.explicit_root = request->explicit_root;
    probe.validate_version = requirement->validate_version;
    probe.diagnostic_sink = request->diagnostic_sink;
    probe.diagnostic_user_data = request->diagnostic_user_data;
    status = umi_toolchain_tool_probe(&probe, &tool, &report);
    slot = umi_toolchain_profile_tool_mutable(profile, requirement->kind);
    if (slot != NULL) {
        *slot = tool;
        slot->required = requirement->required;
    }
    if (report.found) *found += 1U;
    return status;
}

static UmiStatus resolve_compiler(
    UmiToolchainProfile *profile,
    const UmiToolchainScopedDiscoveryRequest *request,
    size_t *found)
{
    UmiToolKind candidates[3];
    size_t index;
    UmiToolchainRequirement requirement;
    const char *preferred = request->preferred_profile;

    if (preferred != NULL && strstr(preferred, "msvc") != NULL) {
        candidates[0] = UMI_TOOL_MSVC_CL; candidates[1] = UMI_TOOL_CLANG; candidates[2] = UMI_TOOL_GCC;
    } else if (preferred != NULL && strstr(preferred, "gcc") != NULL) {
        candidates[0] = UMI_TOOL_GCC; candidates[1] = UMI_TOOL_CLANG; candidates[2] = UMI_TOOL_MSVC_CL;
    } else {
        candidates[0] = UMI_TOOL_CLANG; candidates[1] = UMI_TOOL_GCC; candidates[2] = UMI_TOOL_MSVC_CL;
    }

    for (index = 0U; index < 3U; ++index) {
        UmiStatus status;
        umi_toolchain_requirement_init(&requirement, candidates[index], 1);
        status = probe_into_profile(profile, &requirement, request, found);
        if (status == UMI_STATUS_OK) {
            profile->selected_c_compiler = candidates[index];
            profile->selected_cpp_compiler = candidates[index] == UMI_TOOL_GCC
                ? UMI_TOOL_GXX : candidates[index] == UMI_TOOL_CLANG
                    ? UMI_TOOL_CLANGXX : UMI_TOOL_MSVC_CL;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_toolchain_discover_scoped(
    const UmiToolchainScopedDiscoveryRequest *request,
    UmiToolchainScopedDiscoveryReport *out_report)
{
    size_t index;
    UmiStatus overall = UMI_STATUS_OK;
    UmiToolchainScopedDiscoveryRequest effective_request;
    if (request == NULL || request->operation == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_toolchain_operation_profile_validate(request->operation) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective_request = *request;
    if (effective_request.explicit_root == NULL ||
        effective_request.explicit_root[0] == '\0') {
        const char *environment_root = getenv("UMICOM_TOOLCHAIN_ROOT");
        if (environment_root != NULL && environment_root[0] != '\0') {
            effective_request.explicit_root = environment_root;
        }
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    umi_toolchain_profile_init(&out_report->profile);
    out_report->requirement_count = request->operation->requirements.count;
    if (effective_request.explicit_root != NULL &&
        effective_request.explicit_root[0] != '\0') {
        (void)snprintf(out_report->profile.root, sizeof(out_report->profile.root),
                       "%s", effective_request.explicit_root);
        (void)snprintf(out_report->profile.bin_directory,
                       sizeof(out_report->profile.bin_directory),
                       "%s/bin", effective_request.explicit_root);
        (void)snprintf(out_report->profile.prefix_directory,
                       sizeof(out_report->profile.prefix_directory),
                       "%s", effective_request.explicit_root);
    }

    for (index = 0U; index < request->operation->requirements.count; ++index) {
        const UmiToolchainRequirement *requirement = &request->operation->requirements.items[index];
        UmiStatus status = probe_into_profile(&out_report->profile, requirement,
                                              &effective_request,
                                              &out_report->tools_found);
        if (status != UMI_STATUS_OK && requirement->required) {
            out_report->required_missing += 1U;
            overall = status;
        }
    }

    if (request->operation->requires_compiler) {
        UmiStatus status = resolve_compiler(&out_report->profile,
                                            &effective_request,
                                            &out_report->tools_found);
        if (status != UMI_STATUS_OK) {
            out_report->required_missing += 1U;
            overall = status;
        } else {
            UmiToolchainDiscoveryReport probe_report;
            out_report->compiler_resolved = 1;
            if (request->operation->run_compile_probe) {
                (void)memset(&probe_report, 0, sizeof(probe_report));
                probe_report.profile = out_report->profile;
                status = umi_toolchain_compile_probe(&out_report->profile, &probe_report);
                if (status != UMI_STATUS_OK) {
                    overall = status;
                } else {
                    out_report->compile_probe_passed = 1;
                }
            }
        }
    }

    out_report->complete = out_report->required_missing == 0U &&
        (!request->operation->run_compile_probe || out_report->compile_probe_passed);
    out_report->profile.complete = out_report->complete;
    return out_report->complete ? UMI_STATUS_OK : overall;
}
