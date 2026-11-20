/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/process_fixture.c
 *
 * PURPOSE:
 *   Provide harmless native descendants, argv and output fixtures for tool runs.
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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#endif

static void SleepMilliseconds(unsigned ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    struct timespec delay = {(time_t)(ms / 1000U), (long)(ms % 1000U) * 1000000L};
    (void)nanosleep(&delay, NULL);
#endif
}

static unsigned long ProcessId(void)
{
#ifdef _WIN32
    return (unsigned long)GetCurrentProcessId();
#else
    return (unsigned long)getpid();
#endif
}

static int SpawnMember(const char *self, unsigned depth, unsigned long readyOwner)
{
#ifdef _WIN32
    (void)self;
    wchar_t *program = calloc(32768U, sizeof(wchar_t));
    wchar_t *command = calloc(32832U, sizeof(wchar_t));
    if (program == NULL || command == NULL) { free(program); free(command); return 0; }
    DWORD length = GetModuleFileNameW(NULL, program, 32768U);
    STARTUPINFOW startup;
    PROCESS_INFORMATION process;
    memset(&startup, 0, sizeof(startup)); memset(&process, 0, sizeof(process));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    /* Pass only the test PID used to find the root's reserved marker. */
    int written = readyOwner == 0U ? swprintf(command, 32832U, L"\"%ls\" member %u", program, depth) :
        swprintf(command, 32832U, L"\"%ls\" member %u %lu", program, depth, readyOwner);
    int created = length > 0U && length < 32768U && written > 0 &&
        CreateProcessW(program, command, NULL, NULL, TRUE, CREATE_NO_WINDOW,
            NULL, NULL, &startup, &process);
    if (created) { CloseHandle(process.hThread); CloseHandle(process.hProcess); }
    free(program); free(command);
    return created;
#else
    char count[24];
    (void)snprintf(count, sizeof(count), "%u", depth);
    pid_t child = fork();
    if (child == 0) {
        if (readyOwner == 0U) execl(self, self, "member", count, (char *)NULL);
        else {
            /* The marker contains only an owned fixture's readiness byte. */
            char owner[32];
            (void)snprintf(owner, sizeof owner, "%lu", readyOwner);
            execl(self, self, "member", count, owner, (char *)NULL);
        }
        _exit(125);
    }
    return child > 0;
#endif
}

static int TreeReady(const char *path)
{
    /* Merely finding the reserved file is insufficient. The leaf publishes
     * one byte only after flushing its PID and readiness to the real pipe. */
    for (unsigned attempt = 0U; attempt < 400U; ++attempt) {
        FILE *marker = fopen(path, "rb");
        if (marker != NULL) {
            int found = fgetc(marker) == 'R';
            (void)fclose(marker);
            if (found) return 1;
        }
        SleepMilliseconds(10U);
    }
    return 0;
}

static int MarkTreeReady(const char *path)
{
    /* A Windows reader can briefly hold the marker while polling. Wait for
     * access without creating a file that the root did not reserve. */
    for (unsigned attempt = 0U; attempt < 50U; ++attempt) {
        FILE *signal = fopen(path, "r+b");
        if (signal != NULL) {
            int recorded = fputc('R', signal) == 'R';
            return fclose(signal) == 0 && recorded;
        }
        SleepMilliseconds(10U);
    }
    return 0;
}

static int PrintEnvironment(const char *name)
{
#ifdef _WIN32
    wchar_t key[256];
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name, -1, key, 256) <= 0) return 125;
    wchar_t value[32768];
    SetLastError(ERROR_SUCCESS);
    DWORD count = GetEnvironmentVariableW(key, value, 32768U);
    if (count == 0U && GetLastError() == ERROR_ENVVAR_NOT_FOUND) { puts("MISSING"); return 0; }
    if (count >= 32768U) return 125;
    value[count] = L'\0';
    char text[4U * 32768U];
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value, -1,
            text, (int)sizeof(text), NULL, NULL) <= 0) return 125;
    (void)printf("VALUE=%s\n", text);
#else
    const char *value = getenv(name);
    if (value != NULL) (void)printf("VALUE=%s\n", value); else puts("MISSING");
#endif
    return 0;
}

static int ProgramMain(int argc, char **argv)
{
    if (argc < 2) return 125;
    const char *mode = argv[1];
    if (strcmp(mode, "exit") == 0) return argc > 2 ? atoi(argv[2]) : 0;
    if (strcmp(mode, "echo") == 0) {
        for (int i = 2; i < argc; ++i) {
            (void)printf("ARG[%d]=", i - 2);
            for (const unsigned char *p = (const unsigned char *)argv[i]; *p != 0U; ++p)
                (void)printf("%02x", (unsigned)*p);
            (void)putchar('\n');
        }
        return 0;
    }
    if (strcmp(mode, "diagnostic-tail") == 0) {
        char prefix[4096]; memset(prefix, 'x', sizeof(prefix));
        for (unsigned i = 0U; i < 5U; ++i)
            if (fwrite(prefix, 1U, sizeof(prefix), stdout) != sizeof(prefix)) return 125;
        fputs("\nFINAL_NATIVE_DIAGNOSTIC: source line could not compile\n", stdout);
        return 23;
    }
    if (strcmp(mode, "env") == 0) return argc == 3 ? PrintEnvironment(argv[2]) : 125;
    if (strcmp(mode, "cwd") == 0) {
#ifdef _WIN32
        wchar_t directory[32768]; char text[4U * 32768U];
        DWORD count = GetCurrentDirectoryW(32768U, directory);
        if (count == 0U || count >= 32768U ||
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, directory, -1,
                text, (int)sizeof(text), NULL, NULL) <= 0) return 125;
        puts(text);
#else
        char directory[8192];
        if (getcwd(directory, sizeof(directory)) == NULL) return 125;
        puts(directory);
#endif
        return 0;
    }
    if (strcmp(mode, "tree") == 0 || strcmp(mode, "orphan") == 0 ||
        strcmp(mode, "orphan-fail") == 0 || strcmp(mode, "member") == 0) {
        unsigned depth = strcmp(mode, "member") == 0 && argc > 2 ? (unsigned)atoi(argv[2]) : 2U;
        /* Reserve a per-root marker in the test working directory only for
         * workflows that deliberately return while members remain alive. */
        const int returns = strncmp(mode, "orphan", 6U) == 0;
        const unsigned long readyOwner = returns ? ProcessId() :
            strcmp(mode, "member") == 0 && argc > 3 ? strtoul(argv[3], NULL, 10) : 0UL;
        char marker[64];
        if (readyOwner != 0U) {
            int length = snprintf(marker, sizeof marker, "umicom-fixture-tree-%lu.ready", readyOwner);
            if (length <= 0 || (size_t)length >= sizeof marker) return 125;
        }
        if (returns) {
            FILE *reserved = fopen(marker, "wbx");
            if (reserved == NULL) return 125;
            if (fclose(reserved) != 0) { (void)remove(marker); return 125; }
        }
        (void)printf("MEMBER=%lu\n", ProcessId()); (void)fflush(stdout);
        if (depth != 0U && !SpawnMember(argv[0], depth - 1U, readyOwner)) {
            if (returns) (void)remove(marker);
            return 125;
        }
        if (depth == 0U) {
            puts("TREE_READY");
            if (fflush(stdout) != 0) return 125;
            if (readyOwner != 0U && !MarkTreeReady(marker)) return 125;
        }
        /* The old timed wait assumed the grandchild had run within 700 ms.
         * Keep it for review. The acknowledged leaf now proves the fixture
         * actually reached its three-process topology before the root exits. */
#if 0
        if (strncmp(mode, "orphan", 6U) == 0) {
            SleepMilliseconds(700U);
            return strcmp(mode, "orphan-fail") == 0 ? 17 : 0;
        }
#endif
        if (returns) {
            int ready = TreeReady(marker);
            if (remove(marker) != 0 || !ready) return 125;
            return strcmp(mode, "orphan-fail") == 0 ? 17 : 0;
        }
        SleepMilliseconds(15000U);
        return 0;
    }
    if (strcmp(mode, "flood") == 0 || strcmp(mode, "stream") == 0) {
        char block[4096]; memset(block, 'x', sizeof(block));
        const size_t chunks = strcmp(mode, "flood") == 0 ? 100000U : 321U;
        for (size_t i = 0U; i < chunks; ++i)
            if (fwrite(block, 1U, sizeof(block), stdout) != sizeof(block)) return 125;
        return 0;
    }
    if (strcmp(mode, "sleep") == 0) { SleepMilliseconds(argc > 2 ? (unsigned)atoi(argv[2]) : 100U); return 0; }
    return 125;
}
#include "utf8_entry.inc"
