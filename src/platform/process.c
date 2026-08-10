#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process.c
 *
 * PURPOSE:
 *   Execute child processes through native Windows or POSIX APIs.  Arguments
 *   and environment overrides remain explicit so Umicom tools do not depend
 *   on PowerShell, Python, global PATH changes, or unsafe command concatenation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

static void umi_process_result_init(UmiProcessResult *result)
{
    if (result != NULL) {
        (void)memset(result, 0, sizeof(*result));
        result->exit_code = -1;
    }
}

#ifdef _WIN32
static int umi_windows_needs_quotes(const char *text)
{
    const char *cursor = text;
    if (text == NULL || text[0] == '\0') {
        return 1;
    }
    while (*cursor != '\0') {
        if (*cursor == ' ' || *cursor == '\t' || *cursor == '"') {
            return 1;
        }
        ++cursor;
    }
    return 0;
}

static UmiStatus umi_windows_append_quoted(char *buffer,
                                           size_t capacity,
                                           size_t *used,
                                           const char *text)
{
    size_t backslashes = 0U;
    const char *cursor;
    int quoted;

    if (buffer == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    quoted = umi_windows_needs_quotes(text);
    if (quoted) {
        if (*used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        buffer[(*used)++] = '"';
    }

    for (cursor = text; ; ++cursor) {
        char value = *cursor;
        if (value == '\\') {
            ++backslashes;
            continue;
        }

        if (value == '"') {
            size_t index;
            for (index = 0U; index < backslashes * 2U + 1U; ++index) {
                if (*used + 1U >= capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                buffer[(*used)++] = '\\';
            }
            backslashes = 0U;
            if (*used + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            buffer[(*used)++] = '"';
            continue;
        }

        if (value == '\0') {
            size_t index;
            size_t count = quoted ? backslashes * 2U : backslashes;
            for (index = 0U; index < count; ++index) {
                if (*used + 1U >= capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                buffer[(*used)++] = '\\';
            }
            break;
        }

        while (backslashes > 0U) {
            if (*used + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            buffer[(*used)++] = '\\';
            --backslashes;
        }
        if (*used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        buffer[(*used)++] = value;
    }

    if (quoted) {
        if (*used + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        buffer[(*used)++] = '"';
    }
    buffer[*used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus umi_windows_command_line(const UmiProcessRequest *request,
                                          char *buffer,
                                          size_t capacity)
{
    size_t used = 0U;
    size_t index;
    UmiStatus status;

    status = umi_windows_append_quoted(buffer,
                                       capacity,
                                       &used,
                                       request->program);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    for (index = 0U; index < request->argument_count; ++index) {
        if (used + 2U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        buffer[used++] = ' ';
        buffer[used] = '\0';
        status = umi_windows_append_quoted(buffer,
                                           capacity,
                                           &used,
                                           request->arguments[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

static int umi_windows_env_name_matches(const char *entry,
                                        const char *name)
{
    size_t name_length;
    if (entry == NULL || name == NULL) {
        return 0;
    }
    name_length = strlen(name);
    return _strnicmp(entry, name, name_length) == 0 &&
           entry[name_length] == '=';
}

static char *umi_windows_environment_block(const UmiProcessRequest *request)
{
    LPCH current;
    LPCH cursor;
    size_t total = 1U;
    size_t override_index;
    char *block;
    size_t used = 0U;

    if (request->environment_count == 0U) {
        return NULL;
    }

    current = GetEnvironmentStringsA();
    if (current == NULL) {
        return NULL;
    }

    for (cursor = current; *cursor != '\0'; cursor += strlen(cursor) + 1U) {
        int replaced = 0;
        for (override_index = 0U;
             override_index < request->environment_count;
             ++override_index) {
            if (umi_windows_env_name_matches(
                    cursor,
                    request->environment[override_index].name)) {
                replaced = 1;
                break;
            }
        }
        if (!replaced) {
            total += strlen(cursor) + 1U;
        }
    }

    for (override_index = 0U;
         override_index < request->environment_count;
         ++override_index) {
        total += strlen(request->environment[override_index].name) +
                 strlen(request->environment[override_index].value) + 2U;
    }

    block = (char *)calloc(total, 1U);
    if (block == NULL) {
        FreeEnvironmentStringsA(current);
        return NULL;
    }

    for (cursor = current; *cursor != '\0'; cursor += strlen(cursor) + 1U) {
        int replaced = 0;
        size_t length;
        for (override_index = 0U;
             override_index < request->environment_count;
             ++override_index) {
            if (umi_windows_env_name_matches(
                    cursor,
                    request->environment[override_index].name)) {
                replaced = 1;
                break;
            }
        }
        if (!replaced) {
            length = strlen(cursor) + 1U;
            (void)memcpy(block + used, cursor, length);
            used += length;
        }
    }

    for (override_index = 0U;
         override_index < request->environment_count;
         ++override_index) {
        int written = snprintf(
            block + used,
            total - used,
            "%s=%s",
            request->environment[override_index].name,
            request->environment[override_index].value
        );
        if (written < 0 || (size_t)written + 1U > total - used) {
            free(block);
            FreeEnvironmentStringsA(current);
            return NULL;
        }
        used += (size_t)written + 1U;
    }

    block[used] = '\0';
    FreeEnvironmentStringsA(current);
    return block;
}

static UmiStatus umi_process_execute_windows(const UmiProcessRequest *request,
                                              UmiProcessResult *result)
{
    char command_line[32768];
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    SECURITY_ATTRIBUTES security;
    HANDLE read_pipe = NULL;
    HANDLE write_pipe = NULL;
    char *environment_block;
    BOOL created;
    DWORD exit_code = 1U;
    UmiStatus status;

    status = umi_windows_command_line(request,
                                      command_line,
                                      sizeof(command_line));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(&startup, 0, sizeof(startup));
    (void)memset(&process, 0, sizeof(process));
    startup.cb = sizeof(startup);

    if (request->capture_stdout || request->capture_stderr) {
        (void)memset(&security, 0, sizeof(security));
        security.nLength = sizeof(security);
        security.bInheritHandle = TRUE;
        if (!CreatePipe(&read_pipe, &write_pipe, &security, 0U)) {
            return UMI_STATUS_IO_ERROR;
        }
        (void)SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0U);
        startup.dwFlags |= STARTF_USESTDHANDLES;
        startup.hStdOutput = request->capture_stdout
            ? write_pipe
            : GetStdHandle(STD_OUTPUT_HANDLE);
        startup.hStdError = request->capture_stderr
            ? write_pipe
            : GetStdHandle(STD_ERROR_HANDLE);
        startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    }

    environment_block = umi_windows_environment_block(request);
    created = CreateProcessA(
        NULL,
        command_line,
        NULL,
        NULL,
        TRUE,
        0U,
        environment_block,
        request->working_directory,
        &startup,
        &process
    );
    free(environment_block);

    if (write_pipe != NULL) {
        (void)CloseHandle(write_pipe);
        write_pipe = NULL;
    }

    if (!created) {
        if (read_pipe != NULL) {
            (void)CloseHandle(read_pipe);
        }
        return UMI_STATUS_IO_ERROR;
    }

    result->launched = 1;

    if (read_pipe != NULL) {
        DWORD read_count;
        size_t used = 0U;
        char chunk[4096];
        while (ReadFile(read_pipe,
                        chunk,
                        (DWORD)sizeof(chunk),
                        &read_count,
                        NULL) && read_count > 0U) {
            size_t available = sizeof(result->output) - used - 1U;
            size_t copy_count = (size_t)read_count;
            if (copy_count > available) {
                copy_count = available;
                result->output_truncated = 1;
            }
            if (copy_count > 0U) {
                (void)memcpy(result->output + used, chunk, copy_count);
                used += copy_count;
                result->output[used] = '\0';
            }
        }
        (void)CloseHandle(read_pipe);
    }

    (void)WaitForSingleObject(process.hProcess, INFINITE);
    (void)GetExitCodeProcess(process.hProcess, &exit_code);
    (void)CloseHandle(process.hThread);
    (void)CloseHandle(process.hProcess);
    result->exit_code = (int)exit_code;
    return exit_code == 0U ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

#else

static UmiStatus umi_process_execute_posix(const UmiProcessRequest *request,
                                            UmiProcessResult *result)
{
    int pipe_descriptors[2] = {-1, -1};
    pid_t child;
    char *arguments[UMI_PROCESS_MAX_ARGUMENTS + 2U];
    size_t index;
    int status_code;

    if ((request->capture_stdout || request->capture_stderr) &&
        pipe(pipe_descriptors) != 0) {
        return UMI_STATUS_IO_ERROR;
    }

    arguments[0] = (char *)request->program;
    for (index = 0U; index < request->argument_count; ++index) {
        arguments[index + 1U] = (char *)request->arguments[index];
    }
    arguments[request->argument_count + 1U] = NULL;

    child = fork();
    if (child < 0) {
        if (pipe_descriptors[0] >= 0) {
            (void)close(pipe_descriptors[0]);
            (void)close(pipe_descriptors[1]);
        }
        return UMI_STATUS_IO_ERROR;
    }

    if (child == 0) {
        if (request->working_directory != NULL &&
            chdir(request->working_directory) != 0) {
            _exit(126);
        }

        for (index = 0U; index < request->environment_count; ++index) {
            if (setenv(request->environment[index].name,
                       request->environment[index].value,
                       1) != 0) {
                _exit(126);
            }
        }

        if (pipe_descriptors[1] >= 0) {
            (void)close(pipe_descriptors[0]);
            if (request->capture_stdout) {
                (void)dup2(pipe_descriptors[1], STDOUT_FILENO);
            }
            if (request->capture_stderr) {
                (void)dup2(pipe_descriptors[1], STDERR_FILENO);
            }
            (void)close(pipe_descriptors[1]);
        }

        execvp(request->program, arguments);
        _exit(errno == ENOENT ? 127 : 126);
    }

    result->launched = 1;
    if (pipe_descriptors[1] >= 0) {
        ssize_t count;
        size_t used = 0U;
        char chunk[4096];
        (void)close(pipe_descriptors[1]);
        while ((count = read(pipe_descriptors[0], chunk, sizeof(chunk))) > 0) {
            size_t copy_count = (size_t)count;
            size_t available = sizeof(result->output) - used - 1U;
            if (copy_count > available) {
                copy_count = available;
                result->output_truncated = 1;
            }
            if (copy_count > 0U) {
                (void)memcpy(result->output + used, chunk, copy_count);
                used += copy_count;
                result->output[used] = '\0';
            }
        }
        (void)close(pipe_descriptors[0]);
    }

    if (waitpid(child, &status_code, 0) < 0) {
        return UMI_STATUS_IO_ERROR;
    }

    if (WIFEXITED(status_code)) {
        result->exit_code = WEXITSTATUS(status_code);
    } else if (WIFSIGNALED(status_code)) {
        result->exit_code = 128 + WTERMSIG(status_code);
    } else {
        result->exit_code = -1;
    }

    return result->exit_code == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}
#endif

UmiStatus umi_process_execute(const UmiProcessRequest *request,
                              UmiProcessResult *out_result)
{
    UmiProcessResult local_result;
    UmiProcessResult *result = out_result != NULL ? out_result : &local_result;

    if (request == NULL || request->program == NULL ||
        request->program[0] == '\0' ||
        request->argument_count > UMI_PROCESS_MAX_ARGUMENTS ||
        request->environment_count > UMI_PROCESS_MAX_ENVIRONMENT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_process_result_init(result);
#ifdef _WIN32
    return umi_process_execute_windows(request, result);
#else
    return umi_process_execute_posix(request, result);
#endif
}

UmiStatus umi_process_capture(const char *program,
                              const char *const *arguments,
                              size_t argument_count,
                              char *out_text,
                              size_t capacity,
                              int *out_exit_code)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;
    size_t length;

    if (program == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.capture_stdout = 1;
    request.capture_stderr = 1;

    status = umi_process_execute(&request, &result);
    length = strlen(result.output);
    if (length + 1U > capacity) {
        length = capacity - 1U;
    }
    (void)memcpy(out_text, result.output, length);
    out_text[length] = '\0';
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    return status;
}

UmiStatus umi_process_run(const char *command, int *exit_code)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    const char *arguments[2];
    UmiStatus status;

    if (command == NULL || command[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&request, 0, sizeof(request));
#ifdef _WIN32
    request.program = "cmd.exe";
    arguments[0] = "/C";
    arguments[1] = command;
#else
    request.program = "/bin/sh";
    arguments[0] = "-c";
    arguments[1] = command;
#endif
    request.arguments = arguments;
    request.argument_count = 2U;
    status = umi_process_execute(&request, &result);
    if (exit_code != NULL) {
        *exit_code = result.exit_code;
    }
    return status;
}
