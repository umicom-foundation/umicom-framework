/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process.c
 *
 * PURPOSE:
 *   Execute child processes through native Windows or POSIX APIs.  Arguments
 *   and environment overrides remain explicit so Umicom tools do not depend
 *   on PowerShell, Python, global PATH changes, or unsafe command concatenation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#if defined(__linux__)
#define _GNU_SOURCE
#endif
#endif

#include "umicom/platform/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <windows.h>
#include <wchar.h>
#include <limits.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#endif

/*
 * Initialise process result from caller-provided values so later operations receive a
 * known state.
 */
static void umi_process_result_init(UmiProcessResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result != NULL) {
        (void)memset(result, 0, sizeof(*result));
        result->exit_code = -1;
    }
}

/* Provide the append output operation used by this module and its client applications. */
static void append_output(UmiProcessResult *result,
                          const char *bytes,
                          size_t count)
{
    size_t used;
    size_t capacity;
    size_t overflow;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL || bytes == NULL || count == 0U) return;

    /*
     * Compiler and test failures are normally printed at the end of a child
     * process stream.  Keep that newest evidence when the fixed-size capture
     * buffer fills; retaining only the beginning used to hide the actual
     * diagnostic behind the unhelpful text "Internal error".
     */
    capacity = sizeof(result->output) - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count >= capacity) {
        (void)memcpy(result->output, bytes + (count - capacity), capacity);
        result->output[capacity] = '\0';
        result->output_truncated = 1;
        return;
    }

    used = strlen(result->output);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (used + count > capacity) {
        overflow = used + count - capacity;
        (void)memmove(result->output,
                      result->output + overflow,
                      used - overflow);
        used -= overflow;
        result->output_truncated = 1;
    }
    (void)memcpy(result->output + used, bytes, count);
    result->output[used + count] = '\0';
}

#ifdef _WIN32
/* Add two sizes without allowing an environment or command-line allocation
 * calculation to wrap back to a small value. */
static int umi_process_size_add(size_t left, size_t right, size_t *out)
{
    if (out == NULL || right > SIZE_MAX - left) return 0;
    *out = left + right;
    return 1;
}
#endif

#ifdef _WIN32

/*
 * Provide the monotonic milliseconds operation used by this module and its client
 * applications.
 */
static uint64_t monotonic_milliseconds(void)
{
    return (uint64_t)GetTickCount64();
}

#include "process_windows_arguments.inc"

/* Unicode resource ownership is private to this Windows adapter. */
#include "process_windows_utf8.inc"

/*
 * Provide the drain windows pipe operation used by this module and its client
 * applications.
 */
static void drain_windows_pipe(HANDLE read_pipe, UmiProcessResult *result,
    UmiProcessResultObserver observer, UmiProcessOutputObserver rawObserver, void *context)
{
    DWORD available = 0U;
    char chunk[4096];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (read_pipe == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t chunks = 0U; chunks < 64U; ++chunks) {
        DWORD read_count = 0U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!PeekNamedPipe(read_pipe, NULL, 0U, NULL, &available, NULL) ||
            available == 0U) {
            break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!ReadFile(read_pipe,
                      chunk,
                      available < (DWORD)sizeof(chunk)
                          ? available
                          : (DWORD)sizeof(chunk),
                      &read_count,
                      NULL) || read_count == 0U) {
            break;
        }
        if (rawObserver != NULL) rawObserver(chunk, (size_t)read_count, context);
        append_output(result, chunk, (size_t)read_count);
        /* Publish only between appends, while this thread owns the result. */
        if (observer != NULL) observer(result, context);
    }
}

/*
 * Captured processes are background work by definition: their output belongs
 * in the Studio Output or Terminal pane rather than in a temporary Windows
 * console. Callers can still request a visible window explicitly.
 */
static int umi_windows_process_is_hidden(const UmiProcessRequest *request)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->window_mode == UMI_PROCESS_WINDOW_HIDDEN) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (request->window_mode == UMI_PROCESS_WINDOW_VISIBLE) return 0;
    return request->capture_stdout || request->capture_stderr;
}

/*
 * Provide the process execute windows operation used by this module and its client
 * applications.
 */
static UmiStatus umi_process_execute_windows(const UmiProcessRequest *request,
                                              UmiProcessResult *result,
                                              UmiProcessResultObserver observer, UmiProcessOutputObserver rawObserver,
                                              void *context, UmiProcessLifetime lifetime)
{
    const size_t utf8Capacity = 4U * 32767U + 1U;
    char *command_line = malloc(utf8Capacity);
    wchar_t *wideCommand = NULL, *wideDirectory = NULL, *wideProgram = NULL;
    wchar_t *environment_block = NULL;
    STARTUPINFOEXW startup;
    PROCESS_INFORMATION process;
    SECURITY_ATTRIBUTES security;
    HANDLE read_pipe = NULL, write_pipe = NULL, job = NULL;
    HANDLE inherited[3] = {NULL, NULL, NULL};
    HANDLE standardCopies[3] = {NULL, NULL, NULL};
    size_t inheritedCount = 0U;
    int attributesInitialised = 0;
    int created = 0;
    DWORD creation_flags = CREATE_UNICODE_ENVIRONMENT;
    DWORD exit_code = 1U;
    const uint64_t started = monotonic_milliseconds();
    uint32_t poll_interval = request->poll_interval_ms > 0U ? request->poll_interval_ms : 10U;
    UmiStatus status = UMI_STATUS_OK;
    memset(&startup, 0, sizeof(startup));
    memset(&process, 0, sizeof(process));
    memset(&security, 0, sizeof(security));
    startup.StartupInfo.cb = sizeof(STARTUPINFOW);
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    if (command_line == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    status = umi_windows_command_line(request, command_line, utf8Capacity);
    if (status == UMI_STATUS_OK) status = UmiWindowsUtf16(command_line, &wideCommand);
    if (status == UMI_STATUS_OK) status = UmiWindowsUtf16(request->working_directory, &wideDirectory);
    if (status == UMI_STATUS_OK) status = umi_windows_environment_block(request, &environment_block);
    if (status == UMI_STATUS_OK) status = UmiWindowsProgram(request, &wideProgram);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (umi_windows_process_is_hidden(request)) {
        startup.StartupInfo.dwFlags |= STARTF_USESHOWWINDOW;
        startup.StartupInfo.wShowWindow = (WORD)SW_HIDE;
        creation_flags |= CREATE_NO_WINDOW;
    }
    if (request->capture_stdout || request->capture_stderr) {
        if (!CreatePipe(&read_pipe, &write_pipe, &security, 0U) ||
            !SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0U)) {
            status = UMI_STATUS_IO_ERROR; goto cleanup;
        }
        standardCopies[0] = UmiWindowsStandardHandle(STD_INPUT_HANDLE, 1, &security);
        if (!request->capture_stdout)
            standardCopies[1] = UmiWindowsStandardHandle(STD_OUTPUT_HANDLE, 0, &security);
        if (!request->capture_stderr)
            standardCopies[2] = UmiWindowsStandardHandle(STD_ERROR_HANDLE, 0, &security);
        startup.StartupInfo.hStdInput = standardCopies[0];
        startup.StartupInfo.hStdOutput = request->capture_stdout ? write_pipe : standardCopies[1];
        startup.StartupInfo.hStdError = request->capture_stderr ? write_pipe : standardCopies[2];
        if (startup.StartupInfo.hStdInput == NULL || startup.StartupInfo.hStdOutput == NULL ||
            startup.StartupInfo.hStdError == NULL) { status = UMI_STATUS_IO_ERROR; goto cleanup; }
        inherited[inheritedCount++] = write_pipe;
        for (size_t i = 0U; i < 3U; ++i)
            if (standardCopies[i] != NULL) inherited[inheritedCount++] = standardCopies[i];
        SIZE_T attributeBytes = 0U;
        (void)InitializeProcThreadAttributeList(NULL, 1U, 0U, &attributeBytes);
        if (attributeBytes == 0U) { status = UMI_STATUS_IO_ERROR; goto cleanup; }
        startup.lpAttributeList = malloc(attributeBytes);
        if (startup.lpAttributeList == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
        if (!InitializeProcThreadAttributeList(startup.lpAttributeList, 1U, 0U, &attributeBytes)) {
            status = UMI_STATUS_IO_ERROR; goto cleanup;
        }
        attributesInitialised = 1;
        if (!UpdateProcThreadAttribute(startup.lpAttributeList, 0U,
                PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherited,
                inheritedCount * sizeof(HANDLE), NULL, NULL)) {
            status = UMI_STATUS_IO_ERROR; goto cleanup;
        }
        startup.StartupInfo.cb = sizeof(startup);
        startup.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
        creation_flags |= EXTENDED_STARTUPINFO_PRESENT;
    }
    if (lifetime == UMI_PROCESS_LIFETIME_TREE) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits;
        memset(&limits, 0, sizeof(limits));
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        job = CreateJobObjectW(NULL, NULL);
        if (job == NULL || !SetInformationJobObject(job, JobObjectExtendedLimitInformation,
                &limits, sizeof(limits))) { status = UMI_STATUS_IO_ERROR; goto cleanup; }
        /* No breakaway: a request must not run before ownership is established. */
        creation_flags |= CREATE_SUSPENDED;
    }
    if (umi_cancellation_token_is_requested(request->cancellation)) {
        result->cancelled = 1; status = UMI_STATUS_CANCELLED; goto cleanup;
    }
    created = CreateProcessW(wideProgram, wideCommand, NULL, NULL, write_pipe != NULL,
        creation_flags, environment_block, wideDirectory, &startup.StartupInfo, &process) != 0;
    if (!created) { status = UMI_STATUS_IO_ERROR; goto cleanup; }
    if (write_pipe != NULL) { (void)CloseHandle(write_pipe); write_pipe = NULL; }
    if (job != NULL && (!AssignProcessToJobObject(job, process.hProcess) ||
                       ResumeThread(process.hThread) == (DWORD)-1)) {
        (void)TerminateProcess(process.hProcess, 126U);
        (void)WaitForSingleObject(process.hProcess, INFINITE);
        status = UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    result->launched = 1;
    if (poll_interval > 100U) poll_interval = 100U;
    for (;;) {
        drain_windows_pipe(read_pipe, result, observer, rawObserver, context);
        DWORD waited = WaitForSingleObject(process.hProcess, (DWORD)poll_interval);
        if (waited == WAIT_OBJECT_0) break;
        if (waited == WAIT_FAILED) { status = UMI_STATUS_IO_ERROR; break; }
        if (umi_cancellation_token_is_requested(request->cancellation)) {
            result->cancelled = 1; status = UMI_STATUS_CANCELLED; break;
        }
        if (request->timeout_ms != 0U && monotonic_milliseconds() - started >= request->timeout_ms) {
            result->timed_out = 1; status = UMI_STATUS_TIMEOUT; break;
        }
    }
    if (status != UMI_STATUS_OK) {
        DWORD terminationCode = result->cancelled ? 130U : result->timed_out ? 124U : 126U;
        result->termination_requested = 1;
        /* A successful job termination is already responsible for the root.
         * Issuing a second TerminateProcess can race that asynchronous exit and
         * falsely turn a valid cancellation into an access-denied error. */
        BOOL terminated = job != NULL ? TerminateJobObject(job, terminationCode)
                                     : TerminateProcess(process.hProcess, terminationCode);
        if (!terminated && WaitForSingleObject(process.hProcess, 0U) != WAIT_OBJECT_0) {
            status = UMI_STATUS_IO_ERROR;
            if (job != NULL) { (void)CloseHandle(job); job = NULL; }
            (void)TerminateProcess(process.hProcess, terminationCode);
        }
    }
    (void)WaitForSingleObject(process.hProcess, INFINITE);
    if (!GetExitCodeProcess(process.hProcess, &exit_code)) status = UMI_STATUS_IO_ERROR;
    if (job != NULL) {
        /* A successful root must not leave helpers running after its request. */
        if (!TerminateJobObject(job, 0U)) status = UMI_STATUS_IO_ERROR;
        (void)CloseHandle(job); job = NULL;
    }
    drain_windows_pipe(read_pipe, result, observer, rawObserver, context);
    result->exit_code = (int)exit_code;
    result->duration_ms = monotonic_milliseconds() - started;
    if (status == UMI_STATUS_OK && exit_code != 0U) status = UMI_STATUS_INTERNAL_ERROR;
cleanup:
    if (job != NULL) (void)CloseHandle(job);
    if (created) { (void)CloseHandle(process.hThread); (void)CloseHandle(process.hProcess); }
    if (read_pipe != NULL) (void)CloseHandle(read_pipe);
    if (write_pipe != NULL) (void)CloseHandle(write_pipe);
    for (size_t i = 0U; i < 3U; ++i) if (standardCopies[i] != NULL) (void)CloseHandle(standardCopies[i]);
    if (attributesInitialised) DeleteProcThreadAttributeList(startup.lpAttributeList);
    free(startup.lpAttributeList);
    free(command_line); free(wideCommand); free(wideProgram); free(wideDirectory); free(environment_block);
    return status;
}

#else

/*
 * Provide the monotonic milliseconds operation used by this module and its client
 * applications.
 */
static uint64_t monotonic_milliseconds(void)
{
    struct timespec value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) return 0U;
    return (uint64_t)value.tv_sec * 1000ULL +
           (uint64_t)value.tv_nsec / 1000000ULL;
}

/* Provide the drain posix pipe operation used by this module and its client applications. */
static void drain_posix_pipe(int descriptor, UmiProcessResult *result,
    UmiProcessResultObserver observer, UmiProcessOutputObserver rawObserver, void *context)
{
    char chunk[4096];
    ssize_t count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor < 0) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t chunks = 0U; chunks < 64U; ++chunks) {
        count = read(descriptor, chunk, sizeof(chunk));
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count > 0) {
            /* Keep the OS byte count within the buffer passed to read().
             * This also makes the capture bound explicit to optimising compilers. */
            if ((size_t)count > sizeof(chunk)) return;
            if (rawObserver != NULL) rawObserver(chunk, (size_t)count, context);
            append_output(result, chunk, (size_t)count);
            /* Copying observers see a complete string, never an active append. */
            if (observer != NULL) observer(result, context);
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
        break;
    }
}

/*
 * Provide the process execute posix operation used by this module and its client
 * applications.
 */
#include "process_posix_launch.inc"

static UmiStatus umi_process_execute_posix(const UmiProcessRequest *request,
                                            UmiProcessResult *result,
                                            UmiProcessResultObserver observer, UmiProcessOutputObserver rawObserver,
                                            void *context, UmiProcessLifetime lifetime)
{
    int pipe_descriptors[2] = {-1, -1};
    int errorPipe[2] = {-1, -1};
    pid_t child;
    char *arguments[UMI_PROCESS_MAX_ARGUMENTS + 2U];
    int status_code = 0;
    const uint64_t started = monotonic_milliseconds();
    uint32_t poll_interval = request->poll_interval_ms > 0U ? request->poll_interval_ms : 10U;
    UmiStatus terminal_status;
    UmiPosixLaunch launch;
    int childWaitable = 1;
    int processError = 0;
    /* An ignored SIGCHLD or automatic reaping would discard the root PID
     * before group cleanup can use it safely. Do not launch under that policy. */
    if (lifetime == UMI_PROCESS_LIFETIME_TREE) {
        struct sigaction disposition;
        if (sigaction(SIGCHLD, NULL, &disposition) != 0) return UMI_STATUS_IO_ERROR;
        if (disposition.sa_handler == SIG_IGN || (disposition.sa_flags & SA_NOCLDWAIT) != 0)
            return UMI_STATUS_INVALID_STATE;
    }
    terminal_status = UmiPosixLaunchPrepare(request, &launch);
    if (terminal_status != UMI_STATUS_OK) { UmiPosixLaunchClear(&launch); return terminal_status; }
    if (poll_interval > 100U) poll_interval = 100U;
    if (((request->capture_stdout || request->capture_stderr) && !UmiPosixPipe(pipe_descriptors)) ||
        !UmiPosixPipe(errorPipe)) {
        terminal_status = UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    if ((pipe_descriptors[0] >= 0 && fcntl(pipe_descriptors[0], F_SETFL, O_NONBLOCK) != 0) ||
        fcntl(errorPipe[0], F_SETFL, O_NONBLOCK) != 0) {
        terminal_status = UMI_STATUS_IO_ERROR;
        goto cleanup;
    }
    arguments[0] = (char *)request->program;
    for (size_t index = 0U; index < request->argument_count; ++index)
        arguments[index + 1U] = (char *)request->arguments[index];
    arguments[request->argument_count + 1U] = NULL;
    if (umi_cancellation_token_is_requested(request->cancellation)) {
        result->cancelled = 1; terminal_status = UMI_STATUS_CANCELLED; goto cleanup;
    }
    child = fork();
    if (child < 0) { terminal_status = UMI_STATUS_IO_ERROR; goto cleanup; }
    if (child == 0) {
        (void)close(errorPipe[0]);
        /* Establish ownership before the executable can create descendants. */
        if (lifetime == UMI_PROCESS_LIFETIME_TREE && setpgid(0, 0) != 0)
            UmiPosixChildFail(errorPipe[1], errno);
        if (request->working_directory != NULL && chdir(request->working_directory) != 0)
            UmiPosixChildFail(errorPipe[1], errno);
        if (pipe_descriptors[1] >= 0) {
            (void)close(pipe_descriptors[0]);
            if (request->capture_stdout && dup2(pipe_descriptors[1], STDOUT_FILENO) < 0)
                UmiPosixChildFail(errorPipe[1], errno);
            if (request->capture_stderr && dup2(pipe_descriptors[1], STDERR_FILENO) < 0)
                UmiPosixChildFail(errorPipe[1], errno);
            (void)close(pipe_descriptors[1]);
        }
        int sawAccessDenied = 0;
        int launchError = ENOENT;
        for (size_t index = 0U; index < launch.pathCount; ++index) {
            execve(launch.paths[index], arguments, launch.environment);
            launchError = errno;
            /* Retain execvp's executable-text fallback without allocating or
             * modifying the inherited environment in a multithreaded child. */
            if (launchError == ENOEXEC) {
                char *shellArguments[UMI_PROCESS_MAX_ARGUMENTS + 3U];
                shellArguments[0] = (char *)"/bin/sh";
                shellArguments[1] = launch.paths[index];
                for (size_t n = 0U; n < request->argument_count; ++n)
                    shellArguments[n + 2U] = (char *)request->arguments[n];
                shellArguments[request->argument_count + 2U] = NULL;
                execve(shellArguments[0], shellArguments, launch.environment);
                launchError = errno;
                break;
            }
            if (launchError == EACCES) sawAccessDenied = 1;
            else if (launchError != ENOENT && launchError != ENOTDIR) break;
        }
        if (sawAccessDenied && (launchError == ENOENT || launchError == ENOTDIR)) launchError = EACCES;
        UmiPosixChildFail(errorPipe[1], launchError);
    }
    /* Both sides establish the group. This closes the interval in which a
     * fast cancellation could signal a group the child had not created yet.
     * EACCES means the child already exec'd after its own successful setpgid. */
    if (lifetime == UMI_PROCESS_LIFETIME_TREE && setpgid(child, child) != 0 &&
        errno != EACCES && errno != ESRCH) {
        terminal_status = UMI_STATUS_IO_ERROR;
        (void)kill(child, SIGKILL);
    }
    result->launched = 1;
    (void)close(errorPipe[1]); errorPipe[1] = -1;
    if (pipe_descriptors[1] >= 0) { (void)close(pipe_descriptors[1]); pipe_descriptors[1] = -1; }
    for (;;) {
        siginfo_t information;
        memset(&information, 0, sizeof(information));
        drain_posix_pipe(pipe_descriptors[0], result, observer, rawObserver, context);
        /* WNOWAIT keeps the root PID reserved until its owned process group
         * has been cleaned. Never signal a numeric PID after reaping it. */
        int waited = waitid(P_PID, (id_t)child, &information, WEXITED | WNOHANG | WNOWAIT);
        if (waited < 0) {
            if (errno == EINTR) continue;
            childWaitable = errno != ECHILD;
            terminal_status = UMI_STATUS_IO_ERROR;
            break;
        }
        if (information.si_pid == child) break; /* Completed work wins over late Stop. */
        if (terminal_status != UMI_STATUS_OK) break;
        if (umi_cancellation_token_is_requested(request->cancellation)) {
            result->cancelled = 1; terminal_status = UMI_STATUS_CANCELLED; break;
        }
        if (request->timeout_ms != 0U && monotonic_milliseconds() - started >= request->timeout_ms) {
            result->timed_out = 1; terminal_status = UMI_STATUS_TIMEOUT; break;
        }
        struct timespec duration = {(time_t)(poll_interval / 1000U),
            (long)(poll_interval % 1000U) * 1000000L};
        (void)nanosleep(&duration, NULL);
    }
    if (childWaitable) {
        if (lifetime == UMI_PROCESS_LIFETIME_TREE) {
            if (kill(-child, SIGKILL) != 0 && errno != ESRCH) terminal_status = UMI_STATUS_IO_ERROR;
        }
        if (terminal_status != UMI_STATUS_OK) {
            result->termination_requested = 1;
            if (kill(child, SIGKILL) != 0 && errno != ESRCH) terminal_status = UMI_STATUS_IO_ERROR;
        }
        pid_t reaped;
        do { reaped = waitpid(child, &status_code, 0); } while (reaped < 0 && errno == EINTR);
        if (reaped != child) terminal_status = UMI_STATUS_IO_ERROR;
    }
    ssize_t errorBytes;
    do { errorBytes = read(errorPipe[0], &processError, sizeof(processError)); }
    while (errorBytes < 0 && errno == EINTR);
    if (errorBytes == (ssize_t)sizeof(processError)) {
        result->launched = 0;
        terminal_status = UMI_STATUS_IO_ERROR;
    }
    drain_posix_pipe(pipe_descriptors[0], result, observer, rawObserver, context);
    if (childWaitable && WIFEXITED(status_code)) result->exit_code = WEXITSTATUS(status_code);
    else if (childWaitable && WIFSIGNALED(status_code)) result->exit_code = 128 + WTERMSIG(status_code);
    result->duration_ms = monotonic_milliseconds() - started;
    if (terminal_status == UMI_STATUS_OK && result->exit_code != 0) terminal_status = UMI_STATUS_INTERNAL_ERROR;
cleanup:
    for (size_t i = 0U; i < 2U; ++i) {
        if (pipe_descriptors[i] >= 0) (void)close(pipe_descriptors[i]);
        if (errorPipe[i] >= 0) (void)close(errorPipe[i]);
    }
    UmiPosixLaunchClear(&launch);
    return terminal_status;
}

#endif

/*
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_execute(const UmiProcessRequest *request,
                              UmiProcessResult *out_result)
{
    return UmiProcessExecuteObserved(request, NULL, NULL, out_result);
}

/* Keep validation and platform execution in one path. An observer adds a safe
 * publication boundary; it does not create another child-process runner. */
static UmiStatus UmiProcessExecuteInternal(const UmiProcessRequest *request,
    UmiProcessResultObserver observer, UmiProcessOutputObserver rawObserver,
    void *context, UmiProcessResult *out_result, UmiProcessLifetime lifetime)
{
    UmiProcessResult local_result;
    UmiProcessResult *result = out_result != NULL ? out_result : &local_result;
    size_t index;
    /* Initialise the result before validation so callers never inspect
     * uninitialised output when a malformed request is rejected. */
    umi_process_result_init(result);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request->program == NULL ||
        request->program[0] == '\0' ||
        (lifetime != UMI_PROCESS_LIFETIME_CHILD && lifetime != UMI_PROCESS_LIFETIME_TREE) ||
        request->argument_count > UMI_PROCESS_MAX_ARGUMENTS ||
        request->environment_count > UMI_PROCESS_MAX_ENVIRONMENT ||
        request->window_mode < UMI_PROCESS_WINDOW_INHERIT ||
        request->window_mode > UMI_PROCESS_WINDOW_VISIBLE ||
        (request->argument_count > 0U && request->arguments == NULL) ||
        (request->environment_count > 0U && request->environment == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Reject holes in the argument and environment arrays before the platform
     * adapters turn them into argv or environment blocks.  A NULL argument can
     * otherwise be mistaken for the argv terminator, while a NULL environment
     * value can make setenv or the Windows block builder dereference invalid
     * memory. */
    for (index = 0U; index < request->argument_count; ++index) {
        if (request->arguments[index] == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < request->environment_count; ++index) {
        if (request->environment[index].name == NULL ||
            request->environment[index].value == NULL ||
            request->environment[index].name[0] == '\0' ||
            strchr(request->environment[index].name, '=') != NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* A request stopped before launch must not execute even a short-lived tool. */
    if (umi_cancellation_token_is_requested(request->cancellation)) {
        result->cancelled = 1;
        return UMI_STATUS_CANCELLED;
    }
#ifdef _WIN32
    return umi_process_execute_windows(request, result, observer, rawObserver, context, lifetime);
#else
    return umi_process_execute_posix(request, result, observer, rawObserver, context, lifetime);
#endif
}

/* Compatibility wrapper: retained output snapshots remain unchanged. */
UmiStatus UmiProcessExecuteObserved(const UmiProcessRequest *request,
    UmiProcessResultObserver observer, void *context, UmiProcessResult *outResult)
{
    return UmiProcessExecuteInternal(request, observer, NULL, context, outResult, UMI_PROCESS_LIFETIME_CHILD);
}

/* Raw observers receive bytes before the 64 KiB diagnostic tail can rotate. */
UmiStatus UmiProcessExecuteStreamed(const UmiProcessRequest *request,
    UmiProcessOutputObserver observer, void *context, UmiProcessResult *outResult)
{
    return UmiProcessExecuteInternal(request, NULL, observer, context, outResult, UMI_PROCESS_LIFETIME_CHILD);
}

/* Both observation forms share the same runner and one explicit lifetime. */
UmiStatus UmiProcessExecuteWithLifetime(const UmiProcessRequest *request,
    UmiProcessLifetime lifetime, UmiProcessResultObserver observer,
    UmiProcessOutputObserver rawObserver, void *context, UmiProcessResult *outResult)
{
    return UmiProcessExecuteInternal(request, observer, rawObserver, context, outResult, lifetime);
}

/* Provide the process capture operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = umi_process_execute(&request, &result);
    length = strlen(result.output);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(out_text, result.output, length);
    out_text[length] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = result.exit_code;
    return status;
}

/*
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_run(const char *command, int *exit_code)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    const char *arguments[2];
    UmiStatus status;
    /* Use the shared build helper when it is available from the parent composition. */
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
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = umi_process_execute(&request, &result);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (exit_code != NULL) *exit_code = result.exit_code;
    return status;
}
