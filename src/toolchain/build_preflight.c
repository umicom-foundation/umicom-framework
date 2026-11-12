/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/build_preflight.c
 *
 * PURPOSE:
 *   Admit all required tool and environment metadata before build workflow side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/base/utf8.h"
#include <stdio.h>
#include <string.h>

static UmiStatus PreflightFailure(UmiStatus status, char *message,
    size_t capacity, const char *detail)
{
    if (message != NULL && capacity != 0U)
        (void)snprintf(message, capacity, "%s", detail);
    return status;
}
static UmiStatus PreflightTool(const UmiToolchainProfile *profile,
    UmiToolKind kind, int preview)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    /* Descriptive previews retain the existing default program names when a
     * required slot is absent. Supplied metadata must still be well formed. */
    if (tool == NULL) return preview ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
    if (tool->kind != kind || tool->state < UMI_TOOL_MISSING || tool->state > UMI_TOOL_FAILED)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *end = memchr(tool->path, '\0', sizeof tool->path);
    if (end == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiUtf8Validate(tool->path, (size_t)(end - tool->path), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!preview && (tool->state != UMI_TOOL_VALIDATED || end == tool->path))
        return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

UmiStatus UmiBuildExecutionPreflight(const UmiToolchainProfile *profile,
    const UmiEnvironmentPlan *environment, UmiBuildAction action,
    const UmiBuildRequest *request, const UmiBuildExecutionOptions *options,
    char *message, size_t message_capacity)
{
    UmiStatus status = UmiBuildExecutionValidate(action, request, options, message, message_capacity);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_toolchain_profile_storage_compatible(profile))
        return PreflightFailure(UMI_STATUS_INVALID_ARGUMENT, message, message_capacity,
            "A compatible toolchain profile is required before the workflow starts.");
    if (request->window_mode < UMI_PROCESS_WINDOW_INHERIT ||
        request->window_mode > UMI_PROCESS_WINDOW_VISIBLE)
        return PreflightFailure(UMI_STATUS_INVALID_ARGUMENT, message, message_capacity,
            "The child-window mode is not supported.");
    if (environment != NULL) {
        status = umi_environment_plan_validate(environment);
        if (status != UMI_STATUS_OK)
            return PreflightFailure(status, message, message_capacity,
                "The complete child environment must be valid before the workflow starts.");
    }
    /* Admission considers all phases before clean, log reservation or command
     * notification. It never discovers/probes programs or changes the parent
     * environment. A validated slot remains caller-supplied metadata. */
    int needs_cmake = action == UMI_BUILD_CONFIGURE || action == UMI_BUILD_COMPILE ||
        action == UMI_BUILD_MAKE || action == UMI_BUILD_INSTALL ||
        action == UMI_BUILD_PACKAGE || action == UMI_BUILD_DELIVER ||
        action == UMI_BUILD_COMPILE_TEST;
    int needs_ctest = action == UMI_BUILD_TEST || action == UMI_BUILD_MAKE ||
        action == UMI_BUILD_DELIVER || action == UMI_BUILD_COMPILE_TEST;
    int preview = options != NULL && options->preview;
    if (needs_cmake) {
        status = PreflightTool(profile, UMI_TOOL_CMAKE, preview);
        if (status != UMI_STATUS_OK)
            return PreflightFailure(status, message, message_capacity,
                "CMake metadata is missing or invalid for the requested workflow.");
    }
    if (needs_ctest) {
        status = PreflightTool(profile, UMI_TOOL_CTEST, preview);
        if (status != UMI_STATUS_OK)
            return PreflightFailure(status, message, message_capacity,
                "CTest metadata is missing or invalid for the requested workflow.");
    }
    return PreflightFailure(UMI_STATUS_OK, message, message_capacity,
        "Build metadata is admitted; executable availability and runtime outcomes are not verified.");
}
