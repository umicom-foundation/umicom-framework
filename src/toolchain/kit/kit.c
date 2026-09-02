/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit.c
 * PURPOSE: Implement Framework-owned developer-kit composition.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the tool available operation used by this module and its client applications. */
static int tool_available(const UmiToolInfo *tool)
{
    return tool != NULL &&
           (tool->state == UMI_TOOL_FOUND || tool->state == UMI_TOOL_VALIDATED) &&
           tool->path[0] != '\0';
}

/* Provide the copy tool path operation used by this module and its client applications. */
static void copy_tool_path(char *destination,
                           size_t capacity,
                           const UmiToolInfo *tool)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tool_available(tool)) (void)copy_text(destination, capacity, tool->path);
}

/*
 * Provide the refresh capabilities operation used by this module and its client
 * applications.
 */
static void refresh_capabilities(UmiToolchainKitSnapshot *kit)
{
    UmiToolchainKitCapabilityFlags flags = 0U;
    int cross;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->cmake[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_CONFIGURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->c_compiler[0] != '\0') {
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD;
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_ASSEMBLY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->ctest[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_TEST;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->cmake[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_PACKAGE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->cpp_compiler[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_CPP;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->debugger[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_DEBUG;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (kit->sdk_id[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_SDK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->python[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_PYTHON;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->java[0] != '\0' && kit->javac[0] != '\0')
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_JAVA;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->git[0] != '\0') flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_GIT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((kit->capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_C23) != 0U)
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_C23;

    cross = umi_toolchain_kit_is_cross_compile(kit);
    /* Apply this branch only when its contract condition is satisfied. */
    if (cross != 0) {
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_CROSS_COMPILE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kit->c_compiler[0] != '\0') {
        flags |= UMI_TOOLCHAIN_KIT_CAPABILITY_RUN;
    }
    kit->capabilities = flags;
}

/*
 * Initialise toolchain kit from caller-provided values so later operations receive a known
 * state.
 */
void umi_toolchain_kit_init(UmiToolchainKitSnapshot *kit,
                            const char *kit_id,
                            const char *display_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL) return;
    (void)memset(kit, 0, sizeof(*kit));
    kit->struct_size = (uint32_t)sizeof(*kit);
    kit->api_version = UMI_TOOLCHAIN_KIT_API_VERSION;
    kit->priority = 100U;
    kit->state = UMI_TOOLCHAIN_KIT_UNKNOWN;
    kit->enabled = 1;
    kit->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit_id != NULL) (void)copy_text(kit->id, sizeof(kit->id), kit_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL)
        (void)copy_text(kit->display_name, sizeof(kit->display_name), display_name);
}

/* Check that toolchain kit satisfies its contract before another service relies on it. */
UmiStatus umi_toolchain_kit_validate(const UmiToolchainKitSnapshot *kit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || kit->struct_size < sizeof(*kit) ||
        kit->api_version != UMI_TOOLCHAIN_KIT_API_VERSION ||
        kit->id[0] == '\0' || kit->display_name[0] == '\0' ||
        kit->profile_id[0] == '\0' || kit->target_triple[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (kit->state == UMI_TOOLCHAIN_KIT_READY && kit->c_compiler[0] == '\0')
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit from profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_from_profile(
    UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    const UmiSdkSnapshot *sdk,
    const char *host_triple)
{
    char kit_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    int written;
    const UmiToolInfo *c_compiler;
    const UmiToolInfo *cpp_compiler;
    const UmiToolInfo *linker;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || profile == NULL || profile->profile_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(kit_id, sizeof(kit_id), "kit.%s", profile->profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(kit_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_toolchain_kit_init(kit, kit_id,
                           profile->display_name[0] != '\0'
                               ? profile->display_name : profile->profile_id);
    (void)copy_text(kit->profile_id, sizeof(kit->profile_id), profile->profile_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host_triple != NULL && host_triple[0] != '\0')
        (void)copy_text(kit->host_triple, sizeof(kit->host_triple), host_triple);
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->target_triple[0] != '\0')
        (void)copy_text(kit->target_triple, sizeof(kit->target_triple),
                        profile->target_triple);
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (host_triple != NULL && host_triple[0] != '\0')
        (void)copy_text(kit->target_triple, sizeof(kit->target_triple), host_triple);
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->generator[0] != '\0')
        (void)copy_text(kit->cmake_generator, sizeof(kit->cmake_generator),
                        profile->generator);

    c_compiler = umi_toolchain_profile_c_compiler(profile);
    cpp_compiler = umi_toolchain_profile_cpp_compiler(profile);
    copy_tool_path(kit->c_compiler, sizeof(kit->c_compiler), c_compiler);
    copy_tool_path(kit->cpp_compiler, sizeof(kit->cpp_compiler), cpp_compiler);
    /* Apply this branch only when its contract condition is satisfied. */
    if (kit->c_compiler[0] != '\0')
        (void)copy_text(kit->assembler, sizeof(kit->assembler), kit->c_compiler);
    linker = profile->family == UMI_TOOLCHAIN_MSVC
        ? umi_toolchain_profile_tool(profile, UMI_TOOL_MSVC_LINK) : c_compiler;
    copy_tool_path(kit->linker, sizeof(kit->linker), linker);
    copy_tool_path(kit->debugger, sizeof(kit->debugger),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_GDB));
    copy_tool_path(kit->cmake, sizeof(kit->cmake),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE));
    copy_tool_path(kit->ctest, sizeof(kit->ctest),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_CTEST));
    copy_tool_path(kit->ninja, sizeof(kit->ninja),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_NINJA));
    copy_tool_path(kit->git, sizeof(kit->git),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_GIT));
    copy_tool_path(kit->java, sizeof(kit->java),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_JAVA));
    copy_tool_path(kit->javac, sizeof(kit->javac),
                   umi_toolchain_profile_tool(profile, UMI_TOOL_JAVAC));

    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->c23_capable != 0)
        kit->capabilities |= UMI_TOOLCHAIN_KIT_CAPABILITY_C23;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sdk != NULL && sdk->available != 0) {
        (void)copy_text(kit->sdk_id, sizeof(kit->sdk_id), sdk->id);
    }
    kit->state = profile->complete != 0
        ? UMI_TOOLCHAIN_KIT_READY
        : kit->c_compiler[0] != '\0'
            ? UMI_TOOLCHAIN_KIT_DEGRADED : UMI_TOOLCHAIN_KIT_UNAVAILABLE;
    refresh_capabilities(kit);
    return kit->target_triple[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the toolchain kit apply cross toolchain operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_apply_cross_toolchain(
    UmiToolchainKitSnapshot *kit,
    const UmiCrossToolchainSnapshot *cross_toolchain)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || cross_toolchain == NULL ||
        cross_toolchain->struct_size < sizeof(*cross_toolchain) ||
        cross_toolchain->api_version != UMI_CROSS_TOOLCHAIN_API_VERSION ||
        cross_toolchain->target_triple[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#define COPY_IF_PRESENT(field, source_field)                                        \
    do {                                                                             \
        if ((source_field)[0] != '\0')                                               \
            (void)copy_text(kit->field, sizeof(kit->field), (source_field));          \
    } while (0)
    COPY_IF_PRESENT(target_triple, cross_toolchain->target_triple);
    COPY_IF_PRESENT(c_compiler, cross_toolchain->compiler_path);
    COPY_IF_PRESENT(cpp_compiler, cross_toolchain->cpp_compiler_path);
    COPY_IF_PRESENT(assembler, cross_toolchain->assembler_path);
    COPY_IF_PRESENT(linker, cross_toolchain->linker_path);
    COPY_IF_PRESENT(debugger, cross_toolchain->debugger_path);
    COPY_IF_PRESENT(sysroot, cross_toolchain->sysroot);
#undef COPY_IF_PRESENT
    /* Apply this operation only while the related capability or state is available. */
    if (cross_toolchain->state == UMI_CROSS_TOOLCHAIN_READY)
        kit->state = UMI_TOOLCHAIN_KIT_READY;
    else /* Apply this branch only when its contract condition is satisfied. */ if (cross_toolchain->state == UMI_CROSS_TOOLCHAIN_PARTIAL)
        kit->state = UMI_TOOLCHAIN_KIT_DEGRADED;
    /* Use this fallback path when the earlier condition does not apply. */
    else
        kit->state = UMI_TOOLCHAIN_KIT_UNAVAILABLE;
    kit->revision += 1U;
    refresh_capabilities(kit);
    return cross_toolchain->state == UMI_CROSS_TOOLCHAIN_READY
        ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

/*
 * Provide the toolchain kit apply python runtime operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_apply_python_runtime(
    UmiToolchainKitSnapshot *kit,
    const UmiPythonRuntimeSnapshot *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || runtime == NULL ||
        runtime->struct_size < sizeof(*runtime) ||
        runtime->api_version != UMI_PYTHON_RUNTIME_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->available == 0 || runtime->executable[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy_text(kit->python, sizeof(kit->python), runtime->executable) !=
        UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    kit->revision += 1U;
    refresh_capabilities(kit);
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit set sysroot operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_set_sysroot(UmiToolchainKitSnapshot *kit,
                                       const char *sysroot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || sysroot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(kit->sysroot, sizeof(kit->sysroot), sysroot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) kit->revision += 1U;
    return status;
}

/*
 * Provide the toolchain kit set cmake toolchain file operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_set_cmake_toolchain_file(
    UmiToolchainKitSnapshot *kit,
    const char *path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(kit->cmake_toolchain_file,
                       sizeof(kit->cmake_toolchain_file), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) kit->revision += 1U;
    return status;
}

/*
 * Provide the toolchain kit set environment profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_set_environment_profile(
    UmiToolchainKitSnapshot *kit,
    const char *profile_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(kit->environment_profile,
                       sizeof(kit->environment_profile), profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) kit->revision += 1U;
    return status;
}

/*
 * Provide the toolchain kit is cross compile operation used by this module and its client
 * applications.
 */
int umi_toolchain_kit_is_cross_compile(const UmiToolchainKitSnapshot *kit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (kit == NULL || kit->host_triple[0] == '\0' ||
        kit->target_triple[0] == '\0') return 0;
    return strcmp(kit->host_triple, kit->target_triple) != 0;
}

/*
 * Provide the toolchain kit supports operation used by this module and its client
 * applications.
 */
int umi_toolchain_kit_supports(const UmiToolchainKitSnapshot *kit,
                               UmiToolchainKitCapabilityFlags required)
{
    return kit != NULL && (kit->capabilities & required) == required;
}

/*
 * Provide the toolchain kit state text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_kit_state_text(UmiToolchainKitState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_TOOLCHAIN_KIT_READY: return "READY";
        case UMI_TOOLCHAIN_KIT_DEGRADED: return "DEGRADED";
        case UMI_TOOLCHAIN_KIT_UNAVAILABLE: return "UNAVAILABLE";
        default: return "UNKNOWN";
    }
}
