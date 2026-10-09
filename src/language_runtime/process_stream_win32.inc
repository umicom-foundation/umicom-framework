/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/process_stream_win32.inc
 * PURPOSE: Launch persistent language tools with Unicode paths, literal arguments and explicit pipe ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include "../platform/native_path_internal.h"
#include "umicom/platform/process.h"
/* Reuse the synchronous runner's quoting rules so both launch paths preserve
 * empty arguments, quotes and trailing backslashes in the same way. */
#include "../platform/process_windows_arguments.inc"
#include "../platform/process_windows_environment.inc"

struct UmiLanguageRuntimeProcessStream
{
    PROCESS_INFORMATION process;
    HANDLE input, output;
};

static void LanguageProcessClose(HANDLE *handle)
{
    if (*handle != NULL && *handle != INVALID_HANDLE_VALUE)
        (void)CloseHandle(*handle);
    *handle = NULL;
}

/* Persistent tools now accept a child-only search-path snapshot. Common launch validation selects the executable first; pipe ownership and process lifetime remain with this backend. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig *config,
                                                    UmiLanguageRuntimeProcessStream **out)
{
    UmiStatus status = LanguageProcessValidate(config, out);
    if (status != UMI_STATUS_OK)
        return status;
    wchar_t *command = NULL, *directory = NULL, *program = NULL;
    char *encoded = malloc(4U * 32768U);
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    HANDLE childInput = NULL, childOutput = NULL, childError = NULL;
    STARTUPINFOEXW startup = {0};
    SIZE_T attributeBytes = 0U;
    int attributesReady = 0;
    if (encoded == NULL || stream == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    UmiProcessRequest request = {0};
    request.program = config->program;
    request.arguments = config->arguments;
    request.argument_count = config->argument_count;
    status = umi_windows_command_line(&request, encoded, 4U * 32768U);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(encoded, &command);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(config->program, &program);
    if (status == UMI_STATUS_OK && config->working_directory != NULL && config->working_directory[0] != '\0')
        status = UmiNativePathWide(config->working_directory, &directory);
    if (status != UMI_STATUS_OK)
        goto done;
    /* CreateProcess counts UTF-16 code units, including the terminating NUL.
     * The UTF-8 staging buffer allows supplementary and non-Latin characters. */
    if (wcslen(command) >= 32767U)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    SECURITY_ATTRIBUTES security = {(DWORD)sizeof(security), NULL, TRUE};
    status = UMI_STATUS_IO_ERROR;
    if (!CreatePipe(&childInput, &stream->input, &security, 0U) ||
        !SetHandleInformation(stream->input, HANDLE_FLAG_INHERIT, 0U) ||
        !CreatePipe(&stream->output, &childOutput, &security, 0U) ||
        !SetHandleInformation(stream->output, HANDLE_FLAG_INHERIT, 0U))
        goto done;
    if (!config->merge_stderr)
    {
        HANDLE inherited = GetStdHandle(STD_ERROR_HANDLE);
        if (inherited != NULL && inherited != INVALID_HANDLE_VALUE)
        {
            if (!DuplicateHandle(GetCurrentProcess(), inherited, GetCurrentProcess(), &childError, 0U, TRUE,
                                 DUPLICATE_SAME_ACCESS))
                goto done;
        }
        else
        {
            /* A GUI host may have no stderr. Give the child a valid sink rather
             * than an invalid standard handle; protocol stdout stays separate. */
            childError = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (childError == INVALID_HANDLE_VALUE)
                goto done;
        }
    }
    HANDLE inheritedHandles[3] = {childInput, childOutput, childError};
    SIZE_T handleBytes = (config->merge_stderr ? 2U : 3U) * sizeof(HANDLE);
    (void)InitializeProcThreadAttributeList(NULL, 1U, 0U, &attributeBytes);
    if (attributeBytes == 0U)
        goto done;
    startup.lpAttributeList = malloc(attributeBytes);
    if (startup.lpAttributeList == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    if (!InitializeProcThreadAttributeList(startup.lpAttributeList, 1U, 0U, &attributeBytes))
        goto done;
    attributesReady = 1;
    if (!UpdateProcThreadAttribute(startup.lpAttributeList, 0U, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                   inheritedHandles, handleBytes, NULL, NULL))
        goto done;
    startup.StartupInfo.cb = (DWORD)sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = childInput;
    startup.StartupInfo.hStdOutput = childOutput;
    startup.StartupInfo.hStdError = config->merge_stderr ? childOutput : childError;
    /* A path selects that exact module. A bare name retains Windows' existing
     * executable search rules. Prefer absolute configured paths in a product.
     * Only the three standard streams are inherited from this launch. */
    const wchar_t *module =
        wcschr(program, L'/') != NULL || wcschr(program, L'\\') != NULL || wcschr(program, L':') != NULL
            ? program
            : NULL;
    if (!CreateProcessW(module, command, NULL, NULL, TRUE, CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT,
                        NULL, directory, &startup.StartupInfo, &stream->process))
        goto done;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    if (attributesReady)
        DeleteProcThreadAttributeList(startup.lpAttributeList);
    free(startup.lpAttributeList);
    LanguageProcessClose(&childInput);
    LanguageProcessClose(&childOutput);
    LanguageProcessClose(&childError);
    if (stream != NULL)
    {
        LanguageProcessClose(&stream->input);
        LanguageProcessClose(&stream->output);
        free(stream);
    }
    free(encoded);
    free(command);
    free(program);
    free(directory);
    return status;
}
#endif
/* Native process creation now consumes a validated shared environment plan. The earlier PATH-only launcher is retained for review; native lifetime and stream ownership remain the same. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus LanguageProcessStart(const UmiLanguageRuntimeProcessStreamConfig *config,
                                                    const char *searchPath, UmiLanguageRuntimeProcessStream **out)
{
    UmiStatus status = LanguageProcessValidate(config, out);
    if (status != UMI_STATUS_OK)
        return status;
    wchar_t *command = NULL, *directory = NULL, *program = NULL, *environment = NULL;
    char *encoded = malloc(4U * 32768U);
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    HANDLE childInput = NULL, childOutput = NULL, childError = NULL;
    STARTUPINFOEXW startup = {0};
    SIZE_T attributeBytes = 0U;
    int attributesReady = 0;
    if (encoded == NULL || stream == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    UmiProcessRequest request = {0};
    request.program = config->program;
    /* PATH belongs to this child launch. The caller holds its captured value
     * until native process creation has consumed all borrowed request strings. */
    UmiEnvironmentVariable pathOverride = {"PATH", searchPath};
    if (searchPath != NULL)
    {
        request.environment = &pathOverride;
        request.environment_count = 1U;
    }
    request.arguments = config->arguments;
    request.argument_count = config->argument_count;
    status = umi_windows_environment_block(&request, &environment);
    if (status == UMI_STATUS_OK)
        status = umi_windows_command_line(&request, encoded, 4U * 32768U);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(encoded, &command);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(config->program, &program);
    if (status == UMI_STATUS_OK && config->working_directory != NULL && config->working_directory[0] != '\0')
        status = UmiNativePathWide(config->working_directory, &directory);
    if (status != UMI_STATUS_OK)
        goto done;
    /* CreateProcess counts UTF-16 code units, including the terminating NUL.
     * The UTF-8 staging buffer allows supplementary and non-Latin characters. */
    if (wcslen(command) >= 32767U)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    SECURITY_ATTRIBUTES security = {(DWORD)sizeof(security), NULL, TRUE};
    status = UMI_STATUS_IO_ERROR;
    if (!CreatePipe(&childInput, &stream->input, &security, 0U) ||
        !SetHandleInformation(stream->input, HANDLE_FLAG_INHERIT, 0U) ||
        !CreatePipe(&stream->output, &childOutput, &security, 0U) ||
        !SetHandleInformation(stream->output, HANDLE_FLAG_INHERIT, 0U))
        goto done;
    if (!config->merge_stderr)
    {
        HANDLE inherited = GetStdHandle(STD_ERROR_HANDLE);
        if (inherited != NULL && inherited != INVALID_HANDLE_VALUE)
        {
            if (!DuplicateHandle(GetCurrentProcess(), inherited, GetCurrentProcess(), &childError, 0U, TRUE,
                                 DUPLICATE_SAME_ACCESS))
                goto done;
        }
        else
        {
            /* A GUI host may have no stderr. Give the child a valid sink rather
             * than an invalid standard handle; protocol stdout stays separate. */
            childError = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (childError == INVALID_HANDLE_VALUE)
                goto done;
        }
    }
    HANDLE inheritedHandles[3] = {childInput, childOutput, childError};
    SIZE_T handleBytes = (config->merge_stderr ? 2U : 3U) * sizeof(HANDLE);
    (void)InitializeProcThreadAttributeList(NULL, 1U, 0U, &attributeBytes);
    if (attributeBytes == 0U)
        goto done;
    startup.lpAttributeList = malloc(attributeBytes);
    if (startup.lpAttributeList == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    if (!InitializeProcThreadAttributeList(startup.lpAttributeList, 1U, 0U, &attributeBytes))
        goto done;
    attributesReady = 1;
    if (!UpdateProcThreadAttribute(startup.lpAttributeList, 0U, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                   inheritedHandles, handleBytes, NULL, NULL))
        goto done;
    startup.StartupInfo.cb = (DWORD)sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = childInput;
    startup.StartupInfo.hStdOutput = childOutput;
    startup.StartupInfo.hStdError = config->merge_stderr ? childOutput : childError;
    /* A path selects that exact module. A bare name retains Windows' existing
     * executable search rules. Prefer absolute configured paths in a product.
     * Only the three standard streams are inherited from this launch. */
    const wchar_t *module =
        wcschr(program, L'/') != NULL || wcschr(program, L'\\') != NULL || wcschr(program, L':') != NULL
            ? program
            : NULL;
    if (!CreateProcessW(module, command, NULL, NULL, TRUE, CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT,
                        environment, directory, &startup.StartupInfo, &stream->process))
        goto done;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    if (attributesReady)
        DeleteProcThreadAttributeList(startup.lpAttributeList);
    free(startup.lpAttributeList);
    LanguageProcessClose(&childInput);
    LanguageProcessClose(&childOutput);
    LanguageProcessClose(&childError);
    if (stream != NULL)
    {
        LanguageProcessClose(&stream->input);
        LanguageProcessClose(&stream->output);
        free(stream);
    }
    free(encoded);
    free(command);
    free(program);
    free(directory);
    free(environment);
    return status;
}
#endif
static UmiStatus LanguageProcessStart(const UmiLanguageRuntimeProcessStreamConfig *config,
                                                    const UmiEnvironmentVariable *variables, size_t variable_count, UmiLanguageRuntimeProcessStream **out)
{
    UmiStatus status = LanguageProcessValidate(config, out);
    if (status != UMI_STATUS_OK)
        return status;
    wchar_t *command = NULL, *directory = NULL, *program = NULL, *environment = NULL;
    char *encoded = malloc(4U * 32768U);
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    HANDLE childInput = NULL, childOutput = NULL, childError = NULL;
    STARTUPINFOEXW startup = {0};
    SIZE_T attributeBytes = 0U;
    int attributesReady = 0;
    if (encoded == NULL || stream == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    UmiProcessRequest request = {0};
    request.program = config->program;
    /* PATH belongs to this child launch. The caller holds its captured value
     * until native process creation has consumed all borrowed request strings. */
    request.environment = variables;
    request.environment_count = variable_count;
    request.arguments = config->arguments;
    request.argument_count = config->argument_count;
    status = umi_windows_environment_block(&request, &environment);
    if (status == UMI_STATUS_OK)
        status = umi_windows_command_line(&request, encoded, 4U * 32768U);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(encoded, &command);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(config->program, &program);
    if (status == UMI_STATUS_OK && config->working_directory != NULL && config->working_directory[0] != '\0')
        status = UmiNativePathWide(config->working_directory, &directory);
    if (status != UMI_STATUS_OK)
        goto done;
    /* CreateProcess counts UTF-16 code units, including the terminating NUL.
     * The UTF-8 staging buffer allows supplementary and non-Latin characters. */
    if (wcslen(command) >= 32767U)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    SECURITY_ATTRIBUTES security = {(DWORD)sizeof(security), NULL, TRUE};
    status = UMI_STATUS_IO_ERROR;
    if (!CreatePipe(&childInput, &stream->input, &security, 0U) ||
        !SetHandleInformation(stream->input, HANDLE_FLAG_INHERIT, 0U) ||
        !CreatePipe(&stream->output, &childOutput, &security, 0U) ||
        !SetHandleInformation(stream->output, HANDLE_FLAG_INHERIT, 0U))
        goto done;
    if (!config->merge_stderr)
    {
        HANDLE inherited = GetStdHandle(STD_ERROR_HANDLE);
        if (inherited != NULL && inherited != INVALID_HANDLE_VALUE)
        {
            if (!DuplicateHandle(GetCurrentProcess(), inherited, GetCurrentProcess(), &childError, 0U, TRUE,
                                 DUPLICATE_SAME_ACCESS))
                goto done;
        }
        else
        {
            /* A GUI host may have no stderr. Give the child a valid sink rather
             * than an invalid standard handle; protocol stdout stays separate. */
            childError = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (childError == INVALID_HANDLE_VALUE)
                goto done;
        }
    }
    HANDLE inheritedHandles[3] = {childInput, childOutput, childError};
    SIZE_T handleBytes = (config->merge_stderr ? 2U : 3U) * sizeof(HANDLE);
    (void)InitializeProcThreadAttributeList(NULL, 1U, 0U, &attributeBytes);
    if (attributeBytes == 0U)
        goto done;
    startup.lpAttributeList = malloc(attributeBytes);
    if (startup.lpAttributeList == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    if (!InitializeProcThreadAttributeList(startup.lpAttributeList, 1U, 0U, &attributeBytes))
        goto done;
    attributesReady = 1;
    if (!UpdateProcThreadAttribute(startup.lpAttributeList, 0U, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                   inheritedHandles, handleBytes, NULL, NULL))
        goto done;
    startup.StartupInfo.cb = (DWORD)sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = childInput;
    startup.StartupInfo.hStdOutput = childOutput;
    startup.StartupInfo.hStdError = config->merge_stderr ? childOutput : childError;
    /* A path selects that exact module. A bare name retains Windows' existing
     * executable search rules. Prefer absolute configured paths in a product.
     * Only the three standard streams are inherited from this launch. */
    const wchar_t *module =
        wcschr(program, L'/') != NULL || wcschr(program, L'\\') != NULL || wcschr(program, L':') != NULL
            ? program
            : NULL;
    if (!CreateProcessW(module, command, NULL, NULL, TRUE, CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT,
                        environment, directory, &startup.StartupInfo, &stream->process))
        goto done;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    if (attributesReady)
        DeleteProcThreadAttributeList(startup.lpAttributeList);
    free(startup.lpAttributeList);
    LanguageProcessClose(&childInput);
    LanguageProcessClose(&childOutput);
    LanguageProcessClose(&childError);
    if (stream != NULL)
    {
        LanguageProcessClose(&stream->input);
        LanguageProcessClose(&stream->output);
        free(stream);
    }
    free(encoded);
    free(command);
    free(program);
    free(directory);
    free(environment);
    return status;
}

UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream *stream,
                                                    const void *bytes, size_t length)
{
    if (stream == NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->input == NULL)
        return UMI_STATUS_IO_ERROR;
    const unsigned char *cursor = bytes;
    while (length != 0U)
    {
        DWORD written = 0U;
        DWORD chunk = (DWORD)(length > 16384U ? 16384U : length);
        if (!WriteFile(stream->input, cursor, chunk, &written, NULL) || written == 0U)
            return UMI_STATUS_IO_ERROR;
        cursor += written;
        length -= written;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream *stream, void *buffer,
                                                   size_t capacity, uint32_t timeoutMs, size_t *outRead)
{
    if (outRead != NULL)
        *outRead = 0U;
    if (stream == NULL || buffer == NULL || capacity == 0U || outRead == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ULONGLONG start = GetTickCount64();
    for (;;)
    {
        DWORD available = 0U;
        if (!PeekNamedPipe(stream->output, NULL, 0U, NULL, &available, NULL))
            return GetLastError() == ERROR_BROKEN_PIPE ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
        if (available != 0U)
        {
            DWORD received = 0U;
            DWORD wanted = (DWORD)((size_t)available < capacity ? (size_t)available : capacity);
            if (!ReadFile(stream->output, buffer, wanted, &received, NULL))
                return GetLastError() == ERROR_BROKEN_PIPE ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
            *outRead = received;
            return received == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
        }
        if (GetTickCount64() - start >= timeoutMs)
            return UMI_STATUS_NOT_FOUND;
        Sleep(1U);
    }
}

int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream *stream)
{
    /* Wait reports termination even when the child's exit code equals the
     * Windows STILL_ACTIVE constant; exit-code comparison alone cannot. */
    return stream != NULL && stream->process.hProcess != NULL &&
           WaitForSingleObject(stream->process.hProcess, 0U) == WAIT_TIMEOUT;
}

UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream *stream,
                                                   uint32_t timeoutMs)
{
    if (stream == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    LanguageProcessClose(&stream->input);
    DWORD wait = WaitForSingleObject(stream->process.hProcess, timeoutMs);
    if (wait == WAIT_OBJECT_0)
        return UMI_STATUS_OK;
    if (wait != WAIT_TIMEOUT)
        return UMI_STATUS_IO_ERROR;
    if (!TerminateProcess(stream->process.hProcess, 1U) &&
        WaitForSingleObject(stream->process.hProcess, 0U) != WAIT_OBJECT_0)
        return UMI_STATUS_IO_ERROR;
    return WaitForSingleObject(stream->process.hProcess, 1000U) == WAIT_OBJECT_0 ? UMI_STATUS_OK
                                                                                 : UMI_STATUS_IO_ERROR;
}

void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream *stream)
{
    if (stream == NULL)
        return;
    (void)umi_language_runtime_process_stream_stop(stream, 100U);
    LanguageProcessClose(&stream->input);
    LanguageProcessClose(&stream->output);
    LanguageProcessClose(&stream->process.hThread);
    LanguageProcessClose(&stream->process.hProcess);
    free(stream);
}
