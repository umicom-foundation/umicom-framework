/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/cross_toolchain.c
 * PURPOSE: Implement generic GNU/Clang cross-target tool discovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/cross_toolchain.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/discovery.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
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

/* Provide the make prefixed operation used by this module and its client applications. */
static UmiStatus make_prefixed(char *destination,
                               size_t capacity,
                               const char *prefix,
                               const char *name)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || prefix == NULL || name == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s%s", prefix, name);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the make target argument operation used by this module and its client
 * applications.
 */
static UmiStatus make_target_argument(char *destination,
                                      size_t capacity,
                                      const char *target)
{
    int written;
    written = snprintf(destination, capacity, "--target=%s", target);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Initialise cross toolchain request from caller-provided values so later operations
 * receive a known state.
 */
void umi_cross_toolchain_request_init(UmiCrossToolchainRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_CROSS_TOOLCHAIN_API_VERSION;
    request->family = UMI_CROSS_TOOLCHAIN_GNU;
    request->validate_versions = 1;
}

/*
 * Provide the cross toolchain plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_cross_toolchain_plan(const UmiCrossToolchainRequest *request,
                                   UmiCrossToolchainPlan *out_plan)
{
    UmiStatus status;
    char generated_prefix[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    const char *prefix;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_plan == NULL ||
        request->struct_size < sizeof(*request) ||
        request->api_version != UMI_CROSS_TOOLCHAIN_API_VERSION ||
        request->target_triple == NULL || request->target_triple[0] == '\0' ||
        (request->family != UMI_CROSS_TOOLCHAIN_GNU &&
         request->family != UMI_CROSS_TOOLCHAIN_CLANG)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->struct_size = (uint32_t)sizeof(*out_plan);
    out_plan->api_version = UMI_CROSS_TOOLCHAIN_API_VERSION;
    out_plan->family = request->family;
    status = copy_text(out_plan->target_triple,
                       sizeof(out_plan->target_triple),
                       request->target_triple);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    generated_prefix[0] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->family == UMI_CROSS_TOOLCHAIN_GNU) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (request->tool_prefix != NULL && request->tool_prefix[0] != '\0') {
            prefix = request->tool_prefix;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            int written = snprintf(generated_prefix, sizeof(generated_prefix),
                                   "%s-", request->target_triple);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (written < 0 || (size_t)written >= sizeof(generated_prefix))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            prefix = generated_prefix;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = copy_text(out_plan->tool_prefix,
                                sizeof(out_plan->tool_prefix), prefix)) != UMI_STATUS_OK ||
            (status = make_prefixed(out_plan->compiler,
                                    sizeof(out_plan->compiler), prefix, "gcc")) != UMI_STATUS_OK ||
            (status = make_prefixed(out_plan->cpp_compiler,
                                    sizeof(out_plan->cpp_compiler), prefix, "g++")) != UMI_STATUS_OK ||
            (status = make_prefixed(out_plan->assembler,
                                    sizeof(out_plan->assembler), prefix, "as")) != UMI_STATUS_OK ||
            (status = make_prefixed(out_plan->linker,
                                    sizeof(out_plan->linker), prefix, "ld")) != UMI_STATUS_OK ||
            (status = make_prefixed(out_plan->debugger,
                                    sizeof(out_plan->debugger), prefix, "gdb")) != UMI_STATUS_OK) {
            return status;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        prefix = request->tool_prefix != NULL ? request->tool_prefix : "";
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if ((status = copy_text(out_plan->tool_prefix,
                                sizeof(out_plan->tool_prefix), prefix)) != UMI_STATUS_OK ||
            (status = copy_text(out_plan->compiler,
                                sizeof(out_plan->compiler), "clang")) != UMI_STATUS_OK ||
            (status = copy_text(out_plan->cpp_compiler,
                                sizeof(out_plan->cpp_compiler), "clang++")) != UMI_STATUS_OK ||
            (status = copy_text(out_plan->assembler,
                                sizeof(out_plan->assembler), "clang")) != UMI_STATUS_OK ||
            (status = copy_text(out_plan->linker,
                                sizeof(out_plan->linker), "ld.lld")) != UMI_STATUS_OK ||
            (status = copy_text(out_plan->debugger,
                                sizeof(out_plan->debugger), "lldb")) != UMI_STATUS_OK ||
            (status = make_target_argument(out_plan->target_argument,
                                           sizeof(out_plan->target_argument),
                                           request->target_triple)) != UMI_STATUS_OK) {
            return status;
        }
    }

#define APPLY_OVERRIDE(field, value)                                                \
    do {                                                                             \
        if ((value) != NULL && (value)[0] != '\0') {                                \
            status = copy_text(out_plan->field, sizeof(out_plan->field), (value));   \
            if (status != UMI_STATUS_OK) return status;                              \
        }                                                                            \
    } while (0)
    APPLY_OVERRIDE(compiler, request->compiler_override);
    APPLY_OVERRIDE(cpp_compiler, request->cpp_compiler_override);
    APPLY_OVERRIDE(assembler, request->assembler_override);
    APPLY_OVERRIDE(linker, request->linker_override);
    APPLY_OVERRIDE(debugger, request->debugger_override);
#undef APPLY_OVERRIDE
    return UMI_STATUS_OK;
}

/* Provide the find under root operation used by this module and its client applications. */
static UmiStatus find_under_root(const char *root,
                                 const char *program,
                                 char *out_path,
                                 size_t capacity)
{
    char bin[UMI_TOOL_PATH_CAPACITY];
    char candidate[UMI_TOOL_PATH_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0') return UMI_STATUS_NOT_FOUND;
    status = umi_fs_join(bin, sizeof(bin), root, "bin");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_fs_join(candidate, sizeof(candidate), bin, program);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && umi_fs_is_file(candidate))
            return copy_text(out_path, capacity, candidate);
    }
    status = umi_fs_join(candidate, sizeof(candidate), root, program);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && umi_fs_is_file(candidate))
        return copy_text(out_path, capacity, candidate);
    return UMI_STATUS_NOT_FOUND;
}

/* Provide the resolve program operation used by this module and its client applications. */
static UmiStatus resolve_program(const char *root,
                                 const char *program,
                                 char *out_path,
                                 size_t capacity)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || program[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strchr(program, '/') != NULL || strchr(program, '\\') != NULL) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!umi_fs_is_file(program)) return UMI_STATUS_NOT_FOUND;
        return copy_text(out_path, capacity, program);
    }
    status = find_under_root(root, program, out_path, capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) return status;
#ifdef _WIN32
    {
        char executable[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY + 4U];
        int written;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (strstr(program, ".exe") == NULL) {
            written = snprintf(executable, sizeof(executable), "%s.exe", program);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (written >= 0 && (size_t)written < sizeof(executable)) {
                status = find_under_root(root, executable, out_path, capacity);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) return status;
                status = umi_toolchain_find_on_path(executable, out_path, capacity);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) return status;
            }
        }
    }
#endif
    return umi_toolchain_find_on_path(program, out_path, capacity);
}

/* Provide the capture version operation used by this module and its client applications. */
static UmiStatus capture_version(const char *program,
                                 char *out_version,
                                 size_t capacity)
{
    const char *arguments[] = {"--version"};
    int exit_code = -1;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || program[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_process_capture(program, arguments, 1U,
                                 out_version, capacity, &exit_code);
    first_line(out_version);
    return status == UMI_STATUS_OK && exit_code == 0
        ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

/* Provide the discover slot operation used by this module and its client applications. */
static void discover_slot(const char *root,
                          const char *program,
                          char *path,
                          size_t capacity,
                          int required,
                          size_t *found,
                          size_t *required_count,
                          size_t *missing)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (required != 0) *required_count += 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (resolve_program(root, program, path, capacity) == UMI_STATUS_OK) {
        *found += 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (required != 0) {
        *missing += 1U;
    }
}

/* Provide the capture sysroot operation used by this module and its client applications. */
static void capture_sysroot(const UmiCrossToolchainPlan *plan,
                            const char *compiler,
                            char *out_sysroot,
                            size_t capacity)
{
    const char *arguments[2];
    size_t count = 0U;
    int exit_code = -1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->target_argument[0] != '\0') arguments[count++] = plan->target_argument;
    arguments[count++] = "--print-sysroot";
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_process_capture(compiler, arguments, count,
                            out_sysroot, capacity, &exit_code) == UMI_STATUS_OK &&
        exit_code == 0) {
        first_line(out_sysroot);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_sysroot[0] = '\0';
    }
}

/*
 * Provide the cross toolchain discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_cross_toolchain_discover(const UmiCrossToolchainRequest *request,
                                       UmiCrossToolchainSnapshot *out_snapshot)
{
    UmiCrossToolchainPlan plan;
    UmiStatus status;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_CROSS_TOOLCHAIN_API_VERSION;

    status = umi_cross_toolchain_plan(request, &plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->family = plan.family;
    (void)copy_text(out_snapshot->target_triple,
                    sizeof(out_snapshot->target_triple), plan.target_triple);
    (void)copy_text(out_snapshot->tool_prefix,
                    sizeof(out_snapshot->tool_prefix), plan.tool_prefix);
    (void)copy_text(out_snapshot->target_argument,
                    sizeof(out_snapshot->target_argument), plan.target_argument);
    written = snprintf(out_snapshot->id, sizeof(out_snapshot->id),
                       "%s.%s",
                       plan.family == UMI_CROSS_TOOLCHAIN_GNU ? "gnu" : "clang",
                       plan.target_triple);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_snapshot->id))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    discover_slot(request->explicit_root, plan.compiler,
                  out_snapshot->compiler_path,
                  sizeof(out_snapshot->compiler_path), 1,
                  &out_snapshot->tools_found,
                  &out_snapshot->required_tools,
                  &out_snapshot->required_tools_missing);
    discover_slot(request->explicit_root, plan.cpp_compiler,
                  out_snapshot->cpp_compiler_path,
                  sizeof(out_snapshot->cpp_compiler_path), request->require_cpp,
                  &out_snapshot->tools_found,
                  &out_snapshot->required_tools,
                  &out_snapshot->required_tools_missing);
    discover_slot(request->explicit_root, plan.assembler,
                  out_snapshot->assembler_path,
                  sizeof(out_snapshot->assembler_path), 1,
                  &out_snapshot->tools_found,
                  &out_snapshot->required_tools,
                  &out_snapshot->required_tools_missing);
    discover_slot(request->explicit_root, plan.linker,
                  out_snapshot->linker_path,
                  sizeof(out_snapshot->linker_path), 1,
                  &out_snapshot->tools_found,
                  &out_snapshot->required_tools,
                  &out_snapshot->required_tools_missing);
    discover_slot(request->explicit_root, plan.debugger,
                  out_snapshot->debugger_path,
                  sizeof(out_snapshot->debugger_path), request->require_debugger,
                  &out_snapshot->tools_found,
                  &out_snapshot->required_tools,
                  &out_snapshot->required_tools_missing);

    /* Apply this branch only when its contract condition is satisfied. */
    if (out_snapshot->compiler_path[0] != '\0') {
        capture_sysroot(&plan, out_snapshot->compiler_path,
                        out_snapshot->sysroot, sizeof(out_snapshot->sysroot));
        /* Apply this operation only while the related capability or state is available. */
        if (request->validate_versions != 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (capture_version(out_snapshot->compiler_path,
                                out_snapshot->compiler_version,
                                sizeof(out_snapshot->compiler_version)) !=
                UMI_STATUS_OK) {
                out_snapshot->required_tools_missing += 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (out_snapshot->debugger_path[0] != '\0') {
                (void)capture_version(out_snapshot->debugger_path,
                                      out_snapshot->debugger_version,
                                      sizeof(out_snapshot->debugger_version));
            }
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (out_snapshot->required_tools_missing == 0U) {
        out_snapshot->state = UMI_CROSS_TOOLCHAIN_READY;
        return UMI_STATUS_OK;
    }
    out_snapshot->state = out_snapshot->tools_found > 0U
        ? UMI_CROSS_TOOLCHAIN_PARTIAL : UMI_CROSS_TOOLCHAIN_UNAVAILABLE;
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the cross toolchain family text operation used by this module and its client
 * applications.
 */
const char *umi_cross_toolchain_family_text(UmiCrossToolchainFamily family)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (family) {
        case UMI_CROSS_TOOLCHAIN_GNU: return "GNU";
        case UMI_CROSS_TOOLCHAIN_CLANG: return "Clang/LLVM";
        default: return "Unknown";
    }
}

/*
 * Provide the cross toolchain state text operation used by this module and its client
 * applications.
 */
const char *umi_cross_toolchain_state_text(UmiCrossToolchainState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_CROSS_TOOLCHAIN_READY: return "READY";
        case UMI_CROSS_TOOLCHAIN_PARTIAL: return "PARTIAL";
        case UMI_CROSS_TOOLCHAIN_UNAVAILABLE: return "UNAVAILABLE";
        default: return "UNKNOWN";
    }
}
