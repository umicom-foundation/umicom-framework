/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit.c
 *
 * PURPOSE:
 *   Implement Framework-owned developer-kit composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit.h"

#include <stdio.h>
#include <stdlib.h>
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
/* Return bounded copy failures to the staged constructor.
 * Retain the earlier status-discarding helper for engineering review. */
#if 0
static void copy_tool_path(char *destination,
                           size_t capacity,
                           const UmiToolInfo *tool)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tool_available(tool)) (void)copy_text(destination, capacity, tool->path);
}
#endif
static UmiStatus copy_tool_path(char *destination, size_t capacity, const UmiToolInfo *tool)
{
    if (tool && (tool->state == UMI_TOOL_FOUND || tool->state == UMI_TOOL_VALIDATED) &&
        !memchr(tool->path, '\0', sizeof tool->path))
        return UMI_STATUS_INVALID_ARGUMENT;
    return tool_available(tool) ? copy_text(destination, capacity, tool->path) : UMI_STATUS_OK;
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
/* Bound every field before downstream clients use C-string operations.
 * Retain the earlier minimum-field check for engineering review. */
#if 0
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
#endif
/* Admission is separate from readiness: an initialized kit may be composed
 * incrementally before its profile and target are complete. All text storage
 * must still be bounded before a mutation refreshes capabilities. */
static UmiStatus kit_storage_validate(const UmiToolchainKitSnapshot *kit)
{
    if (!kit || kit->struct_size < sizeof *kit ||
        kit->api_version != UMI_TOOLCHAIN_KIT_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Every inline string belongs to this record. Admit its terminator before
     * catalogue, environment and presentation clients treat it as C text. */
#define KIT_TEXT(field) do { \
    if (!memchr(kit->field, '\0', sizeof kit->field)) return UMI_STATUS_INVALID_ARGUMENT; \
} while (0)
    KIT_TEXT(id); KIT_TEXT(display_name); KIT_TEXT(profile_id); KIT_TEXT(sdk_id);
    KIT_TEXT(host_triple); KIT_TEXT(target_triple); KIT_TEXT(sysroot);
    KIT_TEXT(cmake_generator); KIT_TEXT(cmake_toolchain_file); KIT_TEXT(environment_profile);
    KIT_TEXT(c_compiler); KIT_TEXT(cpp_compiler); KIT_TEXT(assembler); KIT_TEXT(linker);
    KIT_TEXT(debugger); KIT_TEXT(cmake); KIT_TEXT(ctest); KIT_TEXT(ninja);
    KIT_TEXT(git); KIT_TEXT(python); KIT_TEXT(java); KIT_TEXT(javac);
#undef KIT_TEXT
    return UMI_STATUS_OK;
}

static UmiStatus kit_mutation_validate(const UmiToolchainKitSnapshot *kit)
{
    UmiStatus status = kit_storage_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    /* A revision is an observation identity. Never wrap back to an old value. */
    return kit->revision == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus kit_set_text(UmiToolchainKitSnapshot *kit, char *field,
    size_t capacity, const char *text)
{
    if (!text) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = kit_mutation_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    size_t length = 0U;
    while (length < capacity && text[length]) ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A caller may shorten a field using a pointer into that same field. */
    memmove(field, text, length + 1U);
    ++kit->revision;
    return UMI_STATUS_OK;
}

/* The shared storage admission above now serves validation and incremental
 * mutation. Retain the original complete-record validator for review. */
#if 0
UmiStatus umi_toolchain_kit_validate(const UmiToolchainKitSnapshot *kit)
{
    if (!kit || kit->struct_size < sizeof *kit ||
        kit->api_version != UMI_TOOLCHAIN_KIT_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Every inline string belongs to this record. Admit its terminator before
     * catalogue, environment and presentation clients treat it as C text. */
#define KIT_TEXT(field) do { \
    if (!memchr(kit->field, '\0', sizeof kit->field)) return UMI_STATUS_INVALID_ARGUMENT; \
} while (0)
    KIT_TEXT(id); KIT_TEXT(display_name); KIT_TEXT(profile_id); KIT_TEXT(sdk_id);
    KIT_TEXT(host_triple); KIT_TEXT(target_triple); KIT_TEXT(sysroot);
    KIT_TEXT(cmake_generator); KIT_TEXT(cmake_toolchain_file); KIT_TEXT(environment_profile);
    KIT_TEXT(c_compiler); KIT_TEXT(cpp_compiler); KIT_TEXT(assembler); KIT_TEXT(linker);
    KIT_TEXT(debugger); KIT_TEXT(cmake); KIT_TEXT(ctest); KIT_TEXT(ninja);
    KIT_TEXT(git); KIT_TEXT(python); KIT_TEXT(java); KIT_TEXT(javac);
#undef KIT_TEXT
    if (!kit->id[0] || !kit->display_name[0] || !kit->profile_id[0] || !kit->target_triple[0])
        return UMI_STATUS_INVALID_ARGUMENT;
    if (kit->state == UMI_TOOLCHAIN_KIT_READY && !kit->c_compiler[0])
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_toolchain_kit_validate(const UmiToolchainKitSnapshot *kit)
{
    UmiStatus status = kit_storage_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    if (!kit->id[0] || !kit->display_name[0] || !kit->profile_id[0] || !kit->target_triple[0])
        return UMI_STATUS_INVALID_ARGUMENT;
    if (kit->state == UMI_TOOLCHAIN_KIT_READY && !kit->c_compiler[0])
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain kit from profile operation used by this module and its client
 * applications.
 */
/* Build a valid candidate without overwriting the last caller-owned kit.
 * Retain incremental construction below for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_from_profile(
    UmiToolchainKitSnapshot *kit, const UmiToolchainProfile *profile,
    const UmiSdkSnapshot *sdk, const char *host_triple)
{
    if (!kit || !umi_toolchain_profile_storage_compatible(profile))
        return UMI_STATUS_INVALID_ARGUMENT;
#define PROFILE_TEXT(field) do { \
    if (!memchr(profile->field, '\0', sizeof profile->field)) return UMI_STATUS_INVALID_ARGUMENT; \
} while (0)
    PROFILE_TEXT(profile_id); PROFILE_TEXT(display_name);
    PROFILE_TEXT(target_triple); PROFILE_TEXT(generator);
#undef PROFILE_TEXT
    if (!profile->profile_id[0]) return UMI_STATUS_INVALID_ARGUMENT;
    if (sdk && sdk->available && !memchr(sdk->id, '\0', sizeof sdk->id))
        return UMI_STATUS_INVALID_ARGUMENT;
    char kit_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    int written = snprintf(kit_id, sizeof kit_id, "kit.%s", profile->profile_id);
    if (written < 0 || (size_t)written >= sizeof kit_id)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The selected profile and host may refer to existing caller state. Stage
     * an independent snapshot, then publish only a structurally valid kit. */
    UmiToolchainKitSnapshot *candidate = calloc(1U, sizeof *candidate);
    if (!candidate) return UMI_STATUS_OUT_OF_MEMORY;
    umi_toolchain_kit_init(candidate, kit_id,
        profile->display_name[0] ? profile->display_name : profile->profile_id);
    UmiStatus status = UMI_STATUS_OK;
#define COPY(field, text) do { \
    if (status == UMI_STATUS_OK) status = copy_text(candidate->field, sizeof candidate->field, (text)); \
} while (0)
    COPY(profile_id, profile->profile_id);
    if (host_triple && host_triple[0]) COPY(host_triple, host_triple);
    if (profile->target_triple[0]) COPY(target_triple, profile->target_triple);
    else if (host_triple && host_triple[0]) COPY(target_triple, host_triple);
    if (profile->generator[0]) COPY(cmake_generator, profile->generator);
    const UmiToolInfo *c_compiler = umi_toolchain_profile_c_compiler(profile);
    const UmiToolInfo *cpp_compiler = umi_toolchain_profile_cpp_compiler(profile);
    const UmiToolInfo *linker = profile->family == UMI_TOOLCHAIN_MSVC
        ? umi_toolchain_profile_tool(profile, UMI_TOOL_MSVC_LINK) : c_compiler;
#define COPY_TOOL(field, tool) do { \
    if (status == UMI_STATUS_OK) status = copy_tool_path(candidate->field, sizeof candidate->field, (tool)); \
} while (0)
    COPY_TOOL(c_compiler, c_compiler); COPY_TOOL(cpp_compiler, cpp_compiler);
    if (candidate->c_compiler[0]) COPY(assembler, candidate->c_compiler);
    COPY_TOOL(linker, linker);
    COPY_TOOL(debugger, umi_toolchain_profile_tool(profile, UMI_TOOL_GDB));
    COPY_TOOL(cmake, umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE));
    COPY_TOOL(ctest, umi_toolchain_profile_tool(profile, UMI_TOOL_CTEST));
    COPY_TOOL(ninja, umi_toolchain_profile_tool(profile, UMI_TOOL_NINJA));
    COPY_TOOL(git, umi_toolchain_profile_tool(profile, UMI_TOOL_GIT));
    COPY_TOOL(java, umi_toolchain_profile_tool(profile, UMI_TOOL_JAVA));
    COPY_TOOL(javac, umi_toolchain_profile_tool(profile, UMI_TOOL_JAVAC));
#undef COPY_TOOL
    if (sdk && sdk->available) COPY(sdk_id, sdk->id);
#undef COPY
    if (status == UMI_STATUS_OK && !candidate->target_triple[0]) status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK) {
        if (profile->c23_capable) candidate->capabilities |= UMI_TOOLCHAIN_KIT_CAPABILITY_C23;
        candidate->state = profile->complete ? UMI_TOOLCHAIN_KIT_READY :
            candidate->c_compiler[0] ? UMI_TOOLCHAIN_KIT_DEGRADED : UMI_TOOLCHAIN_KIT_UNAVAILABLE;
        refresh_capabilities(candidate);
        status = umi_toolchain_kit_validate(candidate);
    }
    if (status == UMI_STATUS_OK) *kit = *candidate;
    free(candidate);
    return status;
}

/*
 * Provide the toolchain kit apply cross toolchain operation used by this module and its
 * client applications.
 */
/* Stage a bounded cross-target overlay before changing the selected kit.
 * Retain the earlier implementation for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_apply_cross_toolchain(
    UmiToolchainKitSnapshot *kit, const UmiCrossToolchainSnapshot *cross_toolchain)
{
    if (!cross_toolchain || cross_toolchain->struct_size < sizeof *cross_toolchain ||
        cross_toolchain->api_version != UMI_CROSS_TOOLCHAIN_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = kit_mutation_validate(kit);
    if (status != UMI_STATUS_OK) return status;
#define CROSS_TEXT(field) do { \
    if (!memchr(cross_toolchain->field, '\0', sizeof cross_toolchain->field)) return UMI_STATUS_INVALID_ARGUMENT; \
} while (0)
    CROSS_TEXT(target_triple); CROSS_TEXT(compiler_path); CROSS_TEXT(cpp_compiler_path);
    CROSS_TEXT(assembler_path); CROSS_TEXT(linker_path); CROSS_TEXT(debugger_path);
    CROSS_TEXT(sysroot);
#undef CROSS_TEXT
    if (!cross_toolchain->target_triple[0] ||
        (cross_toolchain->state != UMI_CROSS_TOOLCHAIN_READY &&
         cross_toolchain->state != UMI_CROSS_TOOLCHAIN_PARTIAL &&
         cross_toolchain->state != UMI_CROSS_TOOLCHAIN_UNAVAILABLE))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiToolchainKitSnapshot *candidate = calloc(1U, sizeof *candidate);
    if (!candidate) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *kit;
#define COPY_IF_PRESENT(field, source_field) do { \
    if (status == UMI_STATUS_OK && cross_toolchain->source_field[0]) \
        status = copy_text(candidate->field, sizeof candidate->field, cross_toolchain->source_field); \
} while (0)
    COPY_IF_PRESENT(target_triple, target_triple);
    COPY_IF_PRESENT(c_compiler, compiler_path);
    COPY_IF_PRESENT(cpp_compiler, cpp_compiler_path);
    COPY_IF_PRESENT(assembler, assembler_path);
    COPY_IF_PRESENT(linker, linker_path);
    COPY_IF_PRESENT(debugger, debugger_path);
    COPY_IF_PRESENT(sysroot, sysroot);
#undef COPY_IF_PRESENT
    if (status == UMI_STATUS_OK) {
        candidate->state = cross_toolchain->state == UMI_CROSS_TOOLCHAIN_READY
            ? UMI_TOOLCHAIN_KIT_READY : cross_toolchain->state == UMI_CROSS_TOOLCHAIN_PARTIAL
            ? UMI_TOOLCHAIN_KIT_DEGRADED : UMI_TOOLCHAIN_KIT_UNAVAILABLE;
        ++candidate->revision;
        refresh_capabilities(candidate);
        *kit = *candidate;
        /* A valid partial discovery is deliberately published for diagnosis.
         * UNAVAILABLE describes readiness, not a rejected update. */
        status = cross_toolchain->state == UMI_CROSS_TOOLCHAIN_READY
            ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
    }
    free(candidate);
    return status;
}

/*
 * Provide the toolchain kit apply python runtime operation used by this module and its
 * client applications.
 */
/* Admit runtime storage and revision capacity before publishing its executable.
 * Retain the earlier implementation for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_apply_python_runtime(
    UmiToolchainKitSnapshot *kit, const UmiPythonRuntimeSnapshot *runtime)
{
    if (!runtime || runtime->struct_size < sizeof *runtime ||
        runtime->api_version != UMI_PYTHON_RUNTIME_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = kit_mutation_validate(kit);
    if (status != UMI_STATUS_OK) return status;
    if (!runtime->available) return UMI_STATUS_NOT_FOUND;
    if (!memchr(runtime->executable, '\0', sizeof runtime->executable))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!runtime->executable[0]) return UMI_STATUS_NOT_FOUND;
    status = kit_set_text(kit, kit->python, sizeof kit->python, runtime->executable);
    if (status == UMI_STATUS_OK) refresh_capabilities(kit);
    return status;
}

/*
 * Provide the toolchain kit set sysroot operation used by this module and its client
 * applications.
 */
/* Use the shared bounded, overlap-safe mutation contract and checked revision.
 * Retain the earlier implementation for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_set_sysroot(
    UmiToolchainKitSnapshot *kit, const char *sysroot)
{
    if (!kit) return UMI_STATUS_INVALID_ARGUMENT;
    return kit_set_text(kit, kit->sysroot, sizeof kit->sysroot, sysroot);
}

/*
 * Provide the toolchain kit set cmake toolchain file operation used by this module and its
 * client applications.
 */
/* Use the shared bounded, overlap-safe mutation contract and checked revision.
 * Retain the earlier implementation for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_set_cmake_toolchain_file(
    UmiToolchainKitSnapshot *kit, const char *path)
{
    if (!kit) return UMI_STATUS_INVALID_ARGUMENT;
    return kit_set_text(kit, kit->cmake_toolchain_file, sizeof kit->cmake_toolchain_file, path);
}

/*
 * Provide the toolchain kit set environment profile operation used by this module and its
 * client applications.
 */
/* Use the shared bounded, overlap-safe mutation contract and checked revision.
 * Retain the earlier implementation for engineering review. */
#if 0
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
#endif
UmiStatus umi_toolchain_kit_set_environment_profile(
    UmiToolchainKitSnapshot *kit, const char *profile_id)
{
    if (!kit) return UMI_STATUS_INVALID_ARGUMENT;
    return kit_set_text(kit, kit->environment_profile, sizeof kit->environment_profile, profile_id);
}

/*
 * Provide the toolchain kit is cross compile operation used by this module and its client
 * applications.
 */
/* Inspect bounded storage before comparing target names. A partially composed
 * kit may still be queried; retain the former unbounded query for review. */
#if 0
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
#endif
int umi_toolchain_kit_is_cross_compile(const UmiToolchainKitSnapshot *kit)
{
    if (kit_storage_validate(kit) != UMI_STATUS_OK ||
        !kit->host_triple[0] || !kit->target_triple[0]) return 0;
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
