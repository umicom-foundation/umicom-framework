#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/discovery.c
 *
 * PURPOSE:
 *   Implement deterministic native tool discovery for Windows UCRT64 and POSIX
 *   environments, including version validation and a real compile-link-run probe.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/discovery.h"

#include <stddef.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"

static void umi_toolchain_emit(const UmiToolchainDiscoveryRequest *request,
                               UmiDiagnosticSeverity severity,
                               const char *message)
{
    if (request != NULL) {
        umi_diagnostic_emit(request->diagnostic_sink,
                            request->diagnostic_user_data,
                            severity,
                            "toolchain-discovery",
                            message,
                            0U);
    }
}

static int umi_tool_required(UmiToolKind kind,
                             const UmiToolchainDiscoveryRequest *request)
{
    switch (kind) {
        case UMI_TOOL_CMAKE:
        case UMI_TOOL_CTEST:
        case UMI_TOOL_NINJA:
        case UMI_TOOL_GIT:
            return 1;
        case UMI_TOOL_PKG_CONFIG:
            return request == NULL || request->require_gtk ||
                   request->preferred_profile == NULL ||
                   strstr(request->preferred_profile, "msvc") == NULL;
        case UMI_TOOL_WINDRES:
#ifdef _WIN32
            return request == NULL || request->preferred_profile == NULL ||
                   strstr(request->preferred_profile, "msvc") == NULL;
#else
            return 0;
#endif
        case UMI_TOOL_GH:
            return request != NULL && request->require_github_cli;
        case UMI_TOOL_JAVA:
        case UMI_TOOL_JAVAC:
            return request != NULL && request->require_java;
        default:
            return 0;
    }
}

static UmiStatus umi_find_in_directory(const char *directory,
                                       const char *executable,
                                       char *out_path,
                                       size_t capacity)
{
    char candidate[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (directory == NULL || directory[0] == '\0' || executable == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_join(candidate,
                         sizeof(candidate),
                         directory,
                         executable);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (!umi_fs_is_file(candidate)) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (strlen(candidate) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(out_path, capacity, "%s", candidate);
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_find_on_path(const char *executable,
                                     char *out_path,
                                     size_t capacity)
{
    const char *path_value;
    char *copy;
    char *cursor;
    char *save_pointer = NULL;
#ifdef _WIN32
    const char *delimiter = ";";
#else
    const char *delimiter = ":";
#endif

    if (executable == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path_value = getenv("PATH");
    if (path_value == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    copy = (char *)malloc(strlen(path_value) + 1U);
    if (copy == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)strcpy(copy, path_value);

    cursor = strtok_r(copy, delimiter, &save_pointer);
    while (cursor != NULL) {
        UmiStatus status = umi_find_in_directory(cursor,
                                                 executable,
                                                 out_path,
                                                 capacity);
        if (status == UMI_STATUS_OK) {
            free(copy);
            return UMI_STATUS_OK;
        }
        cursor = strtok_r(NULL, delimiter, &save_pointer);
    }
    free(copy);
    return UMI_STATUS_NOT_FOUND;
}

static void umi_toolchain_first_line(char *text)
{
    char *line_end;
    if (text == NULL) {
        return;
    }
    line_end = strpbrk(text, "\r\n");
    if (line_end != NULL) {
        *line_end = '\0';
    }
}

static UmiStatus umi_validate_tool(UmiToolInfo *tool)
{
    const char *arguments[] = {"--version"};
    int exit_code = -1;
    UmiStatus status;

    if (tool == NULL || tool->path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tool->kind == UMI_TOOL_MSVC_CL || tool->kind == UMI_TOOL_MSVC_LINK)
        arguments[0] = "/?";
    else if (tool->kind == UMI_TOOL_VSWHERE)
        arguments[0] = "-help";
    status = umi_process_capture(tool->path,
                                 arguments,
                                 1U,
                                 tool->version,
                                 sizeof(tool->version),
                                 &exit_code);
    umi_toolchain_first_line(tool->version);
    if (exit_code == 0) {
        tool->state = UMI_TOOL_VALIDATED;
        return UMI_STATUS_OK;
    }
    /* Microsoft compiler and linker help may return a non-zero process code
     * even though the executable was launched and identified successfully. */
    if ((tool->kind == UMI_TOOL_MSVC_CL ||
         tool->kind == UMI_TOOL_MSVC_LINK) && tool->version[0] != '\0') {
        tool->state = UMI_TOOL_VALIDATED;
        return UMI_STATUS_OK;
    }
    tool->state = UMI_TOOL_FAILED;
    return status;
}

static void umi_profile_select_root(UmiToolchainProfile *profile,
                                    const UmiToolchainDiscoveryRequest *request)
{
    const char *environment_root = getenv("UMICOM_TOOLCHAIN_ROOT");
#ifdef _WIN32
    const char *const roots[] = {
        "C:\\msys64\\ucrt64",
        "C:\\msys2\\ucrt64",
        "D:\\msys64\\ucrt64",
        NULL
    };
    size_t index;
#endif

    if (request != NULL && request->explicit_root != NULL &&
        request->explicit_root[0] != '\0') {
        (void)snprintf(profile->root,
                       sizeof(profile->root),
                       "%s",
                       request->explicit_root);
    } else if (environment_root != NULL && environment_root[0] != '\0') {
        (void)snprintf(profile->root,
                       sizeof(profile->root),
                       "%s",
                       environment_root);
#ifdef _WIN32
    } else {
        for (index = 0U; roots[index] != NULL; ++index) {
            if (umi_fs_is_directory(roots[index])) {
                (void)snprintf(profile->root,
                               sizeof(profile->root),
                               "%s",
                               roots[index]);
                break;
            }
        }
#endif
    }

    if (profile->root[0] != '\0') {
        (void)umi_fs_join(profile->bin_directory,
                          sizeof(profile->bin_directory),
                          profile->root,
                          "bin");
        (void)snprintf(profile->prefix_directory,
                       sizeof(profile->prefix_directory),
                       "%s",
                       profile->root);
#ifdef _WIN32
        if (request != NULL && request->preferred_profile != NULL &&
            strstr(request->preferred_profile, "msvc") != NULL)
            profile->family = UMI_TOOLCHAIN_MSVC;
        else if (strstr(profile->root, "clang64") != NULL)
            profile->family = UMI_TOOLCHAIN_MSYS2_CLANG64;
        else if (strstr(profile->root, "mingw64") != NULL)
            profile->family = UMI_TOOLCHAIN_MSYS2_MINGW64;
        else
            profile->family = UMI_TOOLCHAIN_MSYS2_UCRT64;
        (void)snprintf(profile->profile_id,
                       sizeof(profile->profile_id),
                       "%s",
                       profile->family == UMI_TOOLCHAIN_MSVC
                           ? "windows-msvc"
                           : profile->family == UMI_TOOLCHAIN_MSYS2_CLANG64
                               ? "windows-clang64-clang"
                               : profile->family == UMI_TOOLCHAIN_MSYS2_MINGW64
                                   ? "windows-mingw64-gcc"
                                   : request != NULL &&
                                     request->preferred_profile != NULL &&
                                     strstr(request->preferred_profile, "gcc") != NULL
                                       ? "windows-ucrt64-gcc"
                                       : "windows-ucrt64-clang");
#else
        profile->family = request != NULL &&
                          request->preferred_profile != NULL &&
                          strstr(request->preferred_profile, "gcc") != NULL
            ? UMI_TOOLCHAIN_POSIX_GCC
            : UMI_TOOLCHAIN_POSIX_CLANG;
        (void)snprintf(profile->profile_id,
                       sizeof(profile->profile_id),
                       "%s",
                       profile->family == UMI_TOOLCHAIN_POSIX_GCC
                           ? "posix-gcc" : "posix-clang");
#endif
    } else {
#ifdef _WIN32
        profile->family = UMI_TOOLCHAIN_UNKNOWN;
        (void)snprintf(profile->profile_id,
                       sizeof(profile->profile_id),
                       "%s",
                       "windows-path");
#else
        profile->family = request != NULL &&
                          request->preferred_profile != NULL &&
                          strstr(request->preferred_profile, "gcc") != NULL
            ? UMI_TOOLCHAIN_POSIX_GCC
            : UMI_TOOLCHAIN_POSIX_CLANG;
        (void)snprintf(profile->profile_id,
                       sizeof(profile->profile_id),
                       "%s",
                       profile->family == UMI_TOOLCHAIN_POSIX_GCC
                           ? "posix-gcc" : "posix-clang");
#endif
    }
    (void)snprintf(profile->display_name,
                   sizeof(profile->display_name),
                   "%s",
                   umi_toolchain_family_text(profile->family));
}

static UmiStatus umi_profile_select_compiler(
    UmiToolchainProfile *profile,
    const UmiToolchainDiscoveryRequest *request)
{
    const char *preferred = request != NULL ? request->preferred_profile : NULL;
    UmiToolKind candidates[3];
    size_t index;
    if (preferred != NULL && strstr(preferred, "msvc") != NULL) {
        candidates[0] = UMI_TOOL_MSVC_CL;
        candidates[1] = UMI_TOOL_CLANG;
        candidates[2] = UMI_TOOL_GCC;
    } else if ((preferred != NULL && strstr(preferred, "gcc") != NULL) ||
               profile->family == UMI_TOOLCHAIN_POSIX_GCC ||
               profile->family == UMI_TOOLCHAIN_MSYS2_MINGW64) {
        candidates[0] = UMI_TOOL_GCC;
        candidates[1] = UMI_TOOL_CLANG;
        candidates[2] = UMI_TOOL_MSVC_CL;
    } else {
        candidates[0] = UMI_TOOL_CLANG;
        candidates[1] = UMI_TOOL_GCC;
        candidates[2] = UMI_TOOL_MSVC_CL;
    }
    for (index = 0U; index < 3U; ++index) {
        UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(
            profile, candidates[index]);
        if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) continue;
        tool->required = 1;
        profile->selected_c_compiler = candidates[index];
        profile->selected_cpp_compiler = candidates[index] == UMI_TOOL_GCC
            ? UMI_TOOL_GXX
            : candidates[index] == UMI_TOOL_CLANG
                ? UMI_TOOL_CLANGXX : UMI_TOOL_MSVC_CL;
        if (candidates[index] == UMI_TOOL_GCC &&
            profile->family == UMI_TOOLCHAIN_POSIX_CLANG)
            profile->family = UMI_TOOLCHAIN_POSIX_GCC;
        if (candidates[index] == UMI_TOOL_MSVC_CL)
            profile->family = UMI_TOOLCHAIN_MSVC;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

static void umi_profile_capture_target(UmiToolchainProfile *profile)
{
    const UmiToolInfo *compiler;
    const char *arguments[] = {"-dumpmachine"};
    int exit_code = -1;
    if (profile == NULL) return;
    if (profile->selected_c_compiler == UMI_TOOL_MSVC_CL) {
        (void)snprintf(profile->target_triple, sizeof(profile->target_triple),
                       "%s", "x86_64-pc-windows-msvc");
        return;
    }
    compiler = umi_toolchain_profile_c_compiler(profile);
    if (compiler == NULL || compiler->state != UMI_TOOL_VALIDATED) return;
    if (umi_process_capture(compiler->path, arguments, 1U,
                            profile->target_triple,
                            sizeof(profile->target_triple), &exit_code) ==
            UMI_STATUS_OK && exit_code == 0) {
        umi_toolchain_first_line(profile->target_triple);
    } else {
        profile->target_triple[0] = '\0';
    }
}

static UmiStatus umi_discover_one(UmiToolchainProfile *profile,
                                  UmiToolKind kind,
                                  int required,
                                  size_t *found_count)
{
    UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(profile, kind);
    const char *executable = umi_tool_default_executable(kind);
    UmiStatus status = UMI_STATUS_NOT_FOUND;

    if (tool == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    tool->required = required;

    if (profile->bin_directory[0] != '\0') {
        status = umi_find_in_directory(profile->bin_directory,
                                       executable,
                                       tool->path,
                                       sizeof(tool->path));
    }
    if (status != UMI_STATUS_OK) {
        status = umi_toolchain_find_on_path(executable,
                                            tool->path,
                                            sizeof(tool->path));
    }

    if (status == UMI_STATUS_OK) {
        tool->state = UMI_TOOL_FOUND;
        *found_count += 1U;
        return umi_validate_tool(tool);
    }
    tool->state = UMI_TOOL_MISSING;
    return required ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}

UmiStatus umi_toolchain_compile_probe(
    const UmiToolchainProfile *profile,
    UmiToolchainDiscoveryReport *in_out_report)
{
    const UmiToolInfo *compiler;
    char temp_root[UMI_PATH_CAPACITY];
    char probe_directory[UMI_PATH_CAPACITY];
    char source_path[UMI_PATH_CAPACITY];
    char executable_path[UMI_PATH_CAPACITY];
    char output_argument[UMI_PATH_CAPACITY + 4U];
    char unique_name[128];
    static atomic_ulong probe_sequence = 0U;
    unsigned long sequence;
    unsigned long process_id;
    const char *arguments[5];
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;

    if (profile == NULL || in_out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    compiler = umi_toolchain_profile_c_compiler(profile);
    if (compiler == NULL || compiler->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_fs_temp_directory(temp_root, sizeof(temp_root));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    sequence = atomic_fetch_add_explicit(&probe_sequence,
                                         1U,
                                         memory_order_relaxed);
#ifdef _WIN32
    process_id = (unsigned long)_getpid();
#else
    process_id = (unsigned long)getpid();
#endif
    (void)snprintf(unique_name,
                   sizeof(unique_name),
                   "umicom-probe-%lu-%lu-%lu",
                   (unsigned long)time(NULL),
                   process_id,
                   sequence);
    status = umi_fs_join(probe_directory,
                         sizeof(probe_directory),
                         temp_root,
                         unique_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_make_directories(probe_directory);
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_join(source_path,
                         sizeof(source_path),
                         probe_directory,
                         "probe.c");
    if (status != UMI_STATUS_OK) return status;
#ifdef _WIN32
    status = umi_fs_join(executable_path,
                         sizeof(executable_path),
                         probe_directory,
                         "probe.exe");
#else
    status = umi_fs_join(executable_path,
                         sizeof(executable_path),
                         probe_directory,
                         "probe");
#endif
    if (status != UMI_STATUS_OK) return status;

    status = umi_fs_write_text(
        source_path,
        "#include <stdio.h>\nint main(void){puts(\"UMICOM_PROBE_OK\");return 0;}\n"
    );
    if (status != UMI_STATUS_OK) return status;

    if (profile->selected_c_compiler == UMI_TOOL_MSVC_CL) {
        (void)snprintf(output_argument, sizeof(output_argument),
                       "/Fe:%s", executable_path);
        arguments[0] = "/nologo";
        arguments[1] = "/std:clatest";
        arguments[2] = source_path;
        arguments[3] = output_argument;
    } else {
        arguments[0] = "-std=c2x";
        arguments[1] = source_path;
        arguments[2] = "-o";
        arguments[3] = executable_path;
    }
    arguments[4] = NULL;
    (void)memset(&request, 0, sizeof(request));
    request.program = compiler->path;
    request.arguments = arguments;
    request.argument_count = 4U;
    request.working_directory = probe_directory;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    if (status != UMI_STATUS_OK || result.exit_code != 0) {
        (void)umi_fs_remove_tree(probe_directory);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    in_out_report->compile_probe_passed = 1;
    in_out_report->link_probe_passed = 1;
    in_out_report->c23_probe_passed = 1;

    (void)memset(&request, 0, sizeof(request));
    request.program = executable_path;
    request.working_directory = probe_directory;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    if (status == UMI_STATUS_OK &&
        result.exit_code == 0 &&
        strstr(result.output, "UMICOM_PROBE_OK") != NULL) {
        in_out_report->runtime_probe_passed = 1;
    }
    (void)umi_fs_remove_tree(probe_directory);
    return in_out_report->runtime_probe_passed
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}

UmiStatus umi_toolchain_discover(
    const UmiToolchainDiscoveryRequest *request,
    UmiToolchainDiscoveryReport *out_report)
{
    size_t index;
    UmiStatus overall = UMI_STATUS_OK;

    if (out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    umi_toolchain_profile_init(&out_report->profile);
    umi_profile_select_root(&out_report->profile, request);

    for (index = 0U; index < UMI_TOOL_COUNT; ++index) {
        int required = umi_tool_required((UmiToolKind)index, request);
        UmiStatus status;
        if (required) {
            out_report->required_tools += 1U;
        }
        status = umi_discover_one(&out_report->profile,
                                  (UmiToolKind)index,
                                  required,
                                  &out_report->tools_found);
        if (required && status != UMI_STATUS_OK) {
            out_report->required_tools_missing += 1U;
            overall = UMI_STATUS_NOT_FOUND;
        }
    }

    out_report->required_tools += 1U; /* One validated C compiler vendor. */
    if (umi_profile_select_compiler(&out_report->profile, request) !=
        UMI_STATUS_OK) {
        out_report->required_tools_missing += 1U;
        overall = UMI_STATUS_NOT_FOUND;
    } else if (request != NULL && request->preferred_profile != NULL) {
        int wants_clang = strstr(request->preferred_profile, "clang") != NULL;
        int wants_gcc = strstr(request->preferred_profile, "gcc") != NULL;
        int wants_msvc = strstr(request->preferred_profile, "msvc") != NULL;
        if ((wants_clang && out_report->profile.selected_c_compiler !=
                UMI_TOOL_CLANG) ||
            (wants_gcc && out_report->profile.selected_c_compiler !=
                UMI_TOOL_GCC) ||
            (wants_msvc && out_report->profile.selected_c_compiler !=
                UMI_TOOL_MSVC_CL)) {
            out_report->required_tools_missing += 1U;
            overall = UMI_STATUS_NOT_FOUND;
        }
    }
    if (out_report->profile.selected_c_compiler < UMI_TOOL_COUNT)
        umi_profile_capture_target(&out_report->profile);

    out_report->profile.complete = out_report->required_tools_missing == 0U;
    if (out_report->profile.complete &&
        (request == NULL || !request->skip_compile_probe)) {
        UmiStatus probe_status = umi_toolchain_compile_probe(
            &out_report->profile,
            out_report
        );
        if (probe_status != UMI_STATUS_OK) {
            overall = probe_status;
            umi_toolchain_emit(request,
                               UMI_DIAGNOSTIC_ERROR,
                               "Compiler compile-link-run probe failed");
        } else {
            out_report->profile.c23_capable =
                out_report->c23_probe_passed;
            umi_toolchain_emit(request,
                               UMI_DIAGNOSTIC_INFO,
                               "Compiler compile-link-run probe passed");
        }
    }
    return overall;
}
