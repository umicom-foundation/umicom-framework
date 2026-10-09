/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/process_stream_posix.inc
 * PURPOSE: Report persistent child launch failures and protect protocol writes from SIGPIPE.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "umicom/platform/process.h"
/* Share the runner's parent-side environment/PATH snapshot and close-on-exec
 * descriptors. No allocator or environment lookup is used after fork. */
#include "../platform/process_posix_launch.inc"

struct UmiLanguageRuntimeProcessStream
{
    pid_t process;
    int input, output;
};

static void LanguageProcessClose(int *descriptor)
{
    if (*descriptor >= 0)
        (void)close(*descriptor);
    *descriptor = -1;
}

/* A socket can suppress SIGPIPE for one write without changing the host's
 * signal handlers. stdin remains a byte stream; language tools must not assume
 * that it is a seekable regular file or a terminal. */
static int LanguageProcessSocket(int descriptors[2])
{
#if defined(__linux__)
    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, descriptors) != 0)
        return 0;
#else
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, descriptors) != 0)
        return 0;
#endif
    for (size_t i = 0U; i < 2U; ++i)
    {
        if (descriptors[i] <= STDERR_FILENO)
        {
            int replacement = fcntl(descriptors[i], F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
            if (replacement < 0)
                goto failed;
            (void)close(descriptors[i]);
            descriptors[i] = replacement;
        }
        if (fcntl(descriptors[i], F_SETFD, FD_CLOEXEC) != 0)
            goto failed;
#if !defined(MSG_NOSIGNAL) && defined(SO_NOSIGPIPE)
        int enabled = 1;
        if (setsockopt(descriptors[i], SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled)) != 0)
            goto failed;
#endif
    }
    return 1;
failed:
    LanguageProcessClose(&descriptors[0]);
    LanguageProcessClose(&descriptors[1]);
    return 0;
}

/* Persistent tools now accept a child-only search-path snapshot. Common launch validation selects the executable first; pipe ownership and process lifetime remain with this backend. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_process_stream_start(const UmiLanguageRuntimeProcessStreamConfig *config,
                                                    UmiLanguageRuntimeProcessStream **out)
{
    UmiStatus status = LanguageProcessValidate(config, out);
    if (status != UMI_STATUS_OK)
        return status;
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE)
    /* Do not install a process-wide SIGPIPE workaround on an unsupported host. */
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    if (stream == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    stream->process = -1;
    stream->input = stream->output = -1;
    int input[2] = {-1, -1}, output[2] = {-1, -1}, launchPipe[2] = {-1, -1};
    UmiPosixLaunch launch = {0};
    UmiProcessRequest request = {0};
    request.program = config->program;
    status = UmiPosixLaunchPrepare(&request, &launch);
    if (status != UMI_STATUS_OK)
        goto done;
    status = UMI_STATUS_IO_ERROR;
    if (!LanguageProcessSocket(input) || !UmiPosixPipe(output) || !UmiPosixPipe(launchPipe))
        goto done;
    char *arguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 2U];
    char *scriptArguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 3U];
    arguments[0] = (char *)config->program;
    scriptArguments[0] = (char *)"/bin/sh";
    for (size_t i = 0U; i < config->argument_count; ++i)
    {
        arguments[i + 1U] = (char *)config->arguments[i];
        scriptArguments[i + 2U] = (char *)config->arguments[i];
    }
    arguments[config->argument_count + 1U] = NULL;
    scriptArguments[config->argument_count + 2U] = NULL;
    pid_t process = fork();
    if (process < 0)
        goto done;
    if (process == 0)
    {
        (void)close(launchPipe[0]);
        if ((config->working_directory != NULL && config->working_directory[0] != '\0' &&
             chdir(config->working_directory) != 0) ||
            dup2(input[1], STDIN_FILENO) < 0 || dup2(output[1], STDOUT_FILENO) < 0 ||
            (config->merge_stderr && dup2(output[1], STDERR_FILENO) < 0))
            UmiPosixChildFail(launchPipe[1], errno);
        (void)close(input[0]);
        (void)close(input[1]);
        (void)close(output[0]);
        (void)close(output[1]);
        int lastError = ENOENT, denied = 0;
        for (size_t i = 0U; i < launch.pathCount; ++i)
        {
            execve(launch.paths[i], arguments, launch.environment);
            lastError = errno;
            /* Retain execvp's historical executable-text-file fallback. There
             * is no constructed shell command; each argument remains literal. */
            if (lastError == ENOEXEC)
            {
                scriptArguments[1] = launch.paths[i];
                execve("/bin/sh", scriptArguments, launch.environment);
                UmiPosixChildFail(launchPipe[1], errno);
            }
            if (lastError == EACCES)
                denied = 1;
            else if (lastError != ENOENT && lastError != ENOTDIR)
                UmiPosixChildFail(launchPipe[1], lastError);
        }
        UmiPosixChildFail(launchPipe[1], denied ? EACCES : lastError);
    }
    stream->process = process;
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[1]);
    /* A successful exec closes this pipe. Child setup failures send errno
     * first, so a missing server or invalid working directory cannot publish
     * a process stream that was never usable. No protocol bytes are consumed. */
    int launchError = 0;
    size_t received = 0U;
    for (;;)
    {
        ssize_t count = read(launchPipe[0], (char *)&launchError + received, sizeof(launchError) - received);
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0)
            goto done;
        if (count == 0)
        {
            if (received != 0U)
                goto done;
            break;
        }
        received += (size_t)count;
        if (received == sizeof(launchError))
            goto done;
    }
    stream->input = input[0];
    input[0] = -1;
    stream->output = output[0];
    output[0] = -1;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    LanguageProcessClose(&input[0]);
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[0]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[0]);
    LanguageProcessClose(&launchPipe[1]);
    UmiPosixLaunchClear(&launch);
    if (stream != NULL)
    {
        if (stream->process > 0)
        {
            (void)kill(stream->process, SIGKILL);
            while (waitpid(stream->process, NULL, 0) < 0 && errno == EINTR)
            {
            }
        }
        free(stream);
    }
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
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE)
    /* Do not install a process-wide SIGPIPE workaround on an unsupported host. */
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    if (stream == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    stream->process = -1;
    stream->input = stream->output = -1;
    int input[2] = {-1, -1}, output[2] = {-1, -1}, launchPipe[2] = {-1, -1};
    UmiPosixLaunch launch = {0};
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
    status = UmiPosixLaunchPrepare(&request, &launch);
    if (status != UMI_STATUS_OK)
        goto done;
    status = UMI_STATUS_IO_ERROR;
    if (!LanguageProcessSocket(input) || !UmiPosixPipe(output) || !UmiPosixPipe(launchPipe))
        goto done;
    char *arguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 2U];
    char *scriptArguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 3U];
    arguments[0] = (char *)config->program;
    scriptArguments[0] = (char *)"/bin/sh";
    for (size_t i = 0U; i < config->argument_count; ++i)
    {
        arguments[i + 1U] = (char *)config->arguments[i];
        scriptArguments[i + 2U] = (char *)config->arguments[i];
    }
    arguments[config->argument_count + 1U] = NULL;
    scriptArguments[config->argument_count + 2U] = NULL;
    pid_t process = fork();
    if (process < 0)
        goto done;
    if (process == 0)
    {
        (void)close(launchPipe[0]);
        if ((config->working_directory != NULL && config->working_directory[0] != '\0' &&
             chdir(config->working_directory) != 0) ||
            dup2(input[1], STDIN_FILENO) < 0 || dup2(output[1], STDOUT_FILENO) < 0 ||
            (config->merge_stderr && dup2(output[1], STDERR_FILENO) < 0))
            UmiPosixChildFail(launchPipe[1], errno);
        (void)close(input[0]);
        (void)close(input[1]);
        (void)close(output[0]);
        (void)close(output[1]);
        int lastError = ENOENT, denied = 0;
        for (size_t i = 0U; i < launch.pathCount; ++i)
        {
            execve(launch.paths[i], arguments, launch.environment);
            lastError = errno;
            /* Retain execvp's historical executable-text-file fallback. There
             * is no constructed shell command; each argument remains literal. */
            if (lastError == ENOEXEC)
            {
                scriptArguments[1] = launch.paths[i];
                execve("/bin/sh", scriptArguments, launch.environment);
                UmiPosixChildFail(launchPipe[1], errno);
            }
            if (lastError == EACCES)
                denied = 1;
            else if (lastError != ENOENT && lastError != ENOTDIR)
                UmiPosixChildFail(launchPipe[1], lastError);
        }
        UmiPosixChildFail(launchPipe[1], denied ? EACCES : lastError);
    }
    stream->process = process;
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[1]);
    /* A successful exec closes this pipe. Child setup failures send errno
     * first, so a missing server or invalid working directory cannot publish
     * a process stream that was never usable. No protocol bytes are consumed. */
    int launchError = 0;
    size_t received = 0U;
    for (;;)
    {
        ssize_t count = read(launchPipe[0], (char *)&launchError + received, sizeof(launchError) - received);
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0)
            goto done;
        if (count == 0)
        {
            if (received != 0U)
                goto done;
            break;
        }
        received += (size_t)count;
        if (received == sizeof(launchError))
            goto done;
    }
    stream->input = input[0];
    input[0] = -1;
    stream->output = output[0];
    output[0] = -1;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    LanguageProcessClose(&input[0]);
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[0]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[0]);
    LanguageProcessClose(&launchPipe[1]);
    UmiPosixLaunchClear(&launch);
    if (stream != NULL)
    {
        if (stream->process > 0)
        {
            (void)kill(stream->process, SIGKILL);
            while (waitpid(stream->process, NULL, 0) < 0 && errno == EINTR)
            {
            }
        }
        free(stream);
    }
    return status;
}
#endif
static UmiStatus LanguageProcessStart(const UmiLanguageRuntimeProcessStreamConfig *config,
                                                    const UmiEnvironmentVariable *variables, size_t variable_count, UmiLanguageRuntimeProcessStream **out)
{
    UmiStatus status = LanguageProcessValidate(config, out);
    if (status != UMI_STATUS_OK)
        return status;
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE)
    /* Do not install a process-wide SIGPIPE workaround on an unsupported host. */
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
    UmiLanguageRuntimeProcessStream *stream = calloc(1U, sizeof(*stream));
    if (stream == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    stream->process = -1;
    stream->input = stream->output = -1;
    int input[2] = {-1, -1}, output[2] = {-1, -1}, launchPipe[2] = {-1, -1};
    UmiPosixLaunch launch = {0};
    UmiProcessRequest request = {0};
    request.program = config->program;
    /* PATH belongs to this child launch. The caller holds its captured value
     * until native process creation has consumed all borrowed request strings. */
    request.environment = variables;
    request.environment_count = variable_count;
    status = UmiPosixLaunchPrepare(&request, &launch);
    if (status != UMI_STATUS_OK)
        goto done;
    status = UMI_STATUS_IO_ERROR;
    if (!LanguageProcessSocket(input) || !UmiPosixPipe(output) || !UmiPosixPipe(launchPipe))
        goto done;
    char *arguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 2U];
    char *scriptArguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 3U];
    arguments[0] = (char *)config->program;
    scriptArguments[0] = (char *)"/bin/sh";
    for (size_t i = 0U; i < config->argument_count; ++i)
    {
        arguments[i + 1U] = (char *)config->arguments[i];
        scriptArguments[i + 2U] = (char *)config->arguments[i];
    }
    arguments[config->argument_count + 1U] = NULL;
    scriptArguments[config->argument_count + 2U] = NULL;
    pid_t process = fork();
    if (process < 0)
        goto done;
    if (process == 0)
    {
        (void)close(launchPipe[0]);
        if ((config->working_directory != NULL && config->working_directory[0] != '\0' &&
             chdir(config->working_directory) != 0) ||
            dup2(input[1], STDIN_FILENO) < 0 || dup2(output[1], STDOUT_FILENO) < 0 ||
            (config->merge_stderr && dup2(output[1], STDERR_FILENO) < 0))
            UmiPosixChildFail(launchPipe[1], errno);
        (void)close(input[0]);
        (void)close(input[1]);
        (void)close(output[0]);
        (void)close(output[1]);
        int lastError = ENOENT, denied = 0;
        for (size_t i = 0U; i < launch.pathCount; ++i)
        {
            execve(launch.paths[i], arguments, launch.environment);
            lastError = errno;
            /* Retain execvp's historical executable-text-file fallback. There
             * is no constructed shell command; each argument remains literal. */
            if (lastError == ENOEXEC)
            {
                scriptArguments[1] = launch.paths[i];
                execve("/bin/sh", scriptArguments, launch.environment);
                UmiPosixChildFail(launchPipe[1], errno);
            }
            if (lastError == EACCES)
                denied = 1;
            else if (lastError != ENOENT && lastError != ENOTDIR)
                UmiPosixChildFail(launchPipe[1], lastError);
        }
        UmiPosixChildFail(launchPipe[1], denied ? EACCES : lastError);
    }
    stream->process = process;
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[1]);
    /* A successful exec closes this pipe. Child setup failures send errno
     * first, so a missing server or invalid working directory cannot publish
     * a process stream that was never usable. No protocol bytes are consumed. */
    int launchError = 0;
    size_t received = 0U;
    for (;;)
    {
        ssize_t count = read(launchPipe[0], (char *)&launchError + received, sizeof(launchError) - received);
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0)
            goto done;
        if (count == 0)
        {
            if (received != 0U)
                goto done;
            break;
        }
        received += (size_t)count;
        if (received == sizeof(launchError))
            goto done;
    }
    stream->input = input[0];
    input[0] = -1;
    stream->output = output[0];
    output[0] = -1;
    *out = stream;
    stream = NULL;
    status = UMI_STATUS_OK;
done:
    LanguageProcessClose(&input[0]);
    LanguageProcessClose(&input[1]);
    LanguageProcessClose(&output[0]);
    LanguageProcessClose(&output[1]);
    LanguageProcessClose(&launchPipe[0]);
    LanguageProcessClose(&launchPipe[1]);
    UmiPosixLaunchClear(&launch);
    if (stream != NULL)
    {
        if (stream->process > 0)
        {
            (void)kill(stream->process, SIGKILL);
            while (waitpid(stream->process, NULL, 0) < 0 && errno == EINTR)
            {
            }
        }
        free(stream);
    }
    return status;
}

UmiStatus umi_language_runtime_process_stream_write(UmiLanguageRuntimeProcessStream *stream,
                                                    const void *bytes, size_t length)
{
    if (stream == NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->input < 0)
        return UMI_STATUS_IO_ERROR;
    const unsigned char *cursor = bytes;
    while (length != 0U)
    {
        size_t chunk = length > 16384U ? 16384U : length;
#ifdef MSG_NOSIGNAL
        ssize_t written = send(stream->input, cursor, chunk, MSG_NOSIGNAL);
#else
        ssize_t written = send(stream->input, cursor, chunk, 0);
#endif
        if (written < 0 && errno == EINTR)
            continue;
        if (written <= 0)
            return UMI_STATUS_IO_ERROR;
        cursor += (size_t)written;
        length -= (size_t)written;
    }
    return UMI_STATUS_OK;
}

static uint64_t LanguageProcessMilliseconds(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0U;
    return (uint64_t)now.tv_sec * 1000U + (uint64_t)now.tv_nsec / 1000000U;
}

UmiStatus umi_language_runtime_process_stream_read(UmiLanguageRuntimeProcessStream *stream, void *buffer,
                                                   size_t capacity, uint32_t timeoutMs, size_t *outRead)
{
    if (outRead != NULL)
        *outRead = 0U;
    if (stream == NULL || buffer == NULL || capacity == 0U || outRead == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t start = LanguageProcessMilliseconds();
    for (;;)
    {
        uint64_t elapsed = LanguageProcessMilliseconds() - start;
        uint64_t remaining = elapsed >= timeoutMs ? 0U : (uint64_t)timeoutMs - elapsed;
        struct pollfd descriptor = {stream->output, POLLIN, 0};
        int ready = poll(&descriptor, 1U, remaining > (uint64_t)INT_MAX ? INT_MAX : (int)remaining);
        if (ready < 0 && errno == EINTR)
        {
            if (LanguageProcessMilliseconds() - start >= timeoutMs)
                return UMI_STATUS_NOT_FOUND;
            continue;
        }
        if (ready == 0)
            return UMI_STATUS_NOT_FOUND;
        if (ready < 0 || (descriptor.revents & POLLNVAL) != 0)
            return UMI_STATUS_IO_ERROR;
        size_t wanted = capacity > 16384U ? 16384U : capacity;
        ssize_t count = read(stream->output, buffer, wanted);
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0)
            return UMI_STATUS_IO_ERROR;
        *outRead = (size_t)count;
        return count == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    }
}

int umi_language_runtime_process_stream_is_running(UmiLanguageRuntimeProcessStream *stream)
{
    if (stream == NULL || stream->process <= 0)
        return 0;
    pid_t result;
    do
    {
        result = waitpid(stream->process, NULL, WNOHANG);
    } while (result < 0 && errno == EINTR);
    if (result == 0)
        return 1;
    if (result == stream->process || (result < 0 && errno == ECHILD))
        stream->process = -1;
    return 0;
}

UmiStatus umi_language_runtime_process_stream_stop(UmiLanguageRuntimeProcessStream *stream,
                                                   uint32_t timeoutMs)
{
    if (stream == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    LanguageProcessClose(&stream->input);
    if (!umi_language_runtime_process_stream_is_running(stream))
        return UMI_STATUS_OK;
    if (kill(stream->process, SIGTERM) != 0 && errno != ESRCH)
        return UMI_STATUS_IO_ERROR;
    uint64_t start = LanguageProcessMilliseconds();
    while (LanguageProcessMilliseconds() - start < timeoutMs)
    {
        if (!umi_language_runtime_process_stream_is_running(stream))
            return UMI_STATUS_OK;
        struct timespec delay = {0, 1000000L};
        (void)nanosleep(&delay, NULL);
    }
    if (!umi_language_runtime_process_stream_is_running(stream))
        return UMI_STATUS_OK;
    if (kill(stream->process, SIGKILL) != 0 && errno != ESRCH)
        return UMI_STATUS_IO_ERROR;
    pid_t result;
    do
    {
        result = waitpid(stream->process, NULL, 0);
    } while (result < 0 && errno == EINTR);
    if (result != stream->process && !(result < 0 && errno == ECHILD))
        return UMI_STATUS_IO_ERROR;
    stream->process = -1;
    return UMI_STATUS_OK;
}

void umi_language_runtime_process_stream_destroy(UmiLanguageRuntimeProcessStream *stream)
{
    if (stream == NULL)
        return;
    (void)umi_language_runtime_process_stream_stop(stream, 100U);
    LanguageProcessClose(&stream->input);
    LanguageProcessClose(&stream->output);
    free(stream);
}
