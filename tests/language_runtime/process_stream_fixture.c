/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/process_stream_fixture.c
 * PURPOSE: Provide a literal-argument and byte-stream child for native language transport regression checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
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
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <time.h>
#endif

static void Hex(const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p)
        printf("%02x", (unsigned)*p);
    putchar('\n');
}

#include "language_server_fixture.inc"
#include "completion_server_fixture.inc"
#include "formatting_server_fixture.inc"
#include "range_formatting_server_fixture.inc"
#include "hover_server_fixture.inc"
#include "navigation_server_fixture.inc"
#include "selection_server_fixture.inc"
#include "folding_server_fixture.inc"
#include "call_server_fixture.inc"
#include "type_navigation_server_fixture.inc"
#include "symbol_server_fixture.inc"
#include "workspace_symbol_server_fixture.inc"
#include "signature_server_fixture.inc"
#include "rename_server_fixture.inc"
#include "rename_sources_fixture.inc"
#include "action_sources_fixture.inc"
#include "action_pull_context_fixture.inc"
#include "action_filter_fixture.inc"
#include "action_resolution_fixture.inc"
#include "code_action_server_fixture.inc"
#include "diagnostic_related_fixture.inc"
#include "diagnostic_server_fixture.inc"
#include "pull_diagnostic_server_fixture.inc"

static int ProgramMain(int argc, char **argv)
{
    if (argc < 2)
        return 2;
#ifdef _WIN32
    /* Protocol headers already contain CRLF. Binary stdout prevents the CRT
     * from inserting another carriage return and corrupting the framing. */
    if (_setmode(_fileno(stdout), _O_BINARY) < 0)
        return 3;
    if (_setmode(_fileno(stdin), _O_BINARY) < 0)
        return 3;
#endif
    const char *mode = argv[1];
    if (strncmp(mode, "calls-", 6U) == 0)
        return CallHierarchyFixture(mode);
    if (strncmp(mode, "pull-diagnostics-", 17U) == 0)
        return PullDiagnosticFixture(mode);
    if (strncmp(mode, "diagnostics-", 12U) == 0)
        return DiagnosticFixture(mode);
    if (strncmp(mode, "actions-resolve-", 16U) == 0)
        return ActionResolutionFixture(mode);
    if (strncmp(mode, "actions-filter-", 15U) == 0)
        return ActionFilterFixture(mode);
    if (strncmp(mode, "actions-pull-", 13U) == 0)
        return ActionPullContextFixture(mode);
    if (strncmp(mode, "actions-sources-", 16U) == 0)
        return ActionSourcesFixture(mode);
    if (strncmp(mode, "actions-", 8U) == 0)
        return CodeActionFixture(mode);
    if (strncmp(mode, "rename-sources-", 15U) == 0)
        return RenameSourcesFixture(mode);
    if (strncmp(mode, "rename-", 7U) == 0)
        return RenameFixture(mode);
    if (strncmp(mode, "signature-", 10U) == 0)
        return SignatureFixture(mode);
    if (strncmp(mode, "symbols-", 8U) == 0)
        return SymbolFixture(mode);
    if (strncmp(mode, "workspace-symbols-", 18U) == 0)
        return WorkspaceSymbolFixture(mode);
    if (strncmp(mode,"foldings-",9U)==0)
        return FoldingRangeFixture(mode);
    if (strncmp(mode, "selections-", 11U) == 0)
        return SelectionRangeFixture(mode);
    if (strncmp(mode, "navigation-", 11U) == 0)
        return NavigationFixture(mode);
    if (strncmp(mode, "type-navigation-", 16U) == 0)
        return TypeNavigationFixture(mode + 5U, 0);
    if (strncmp(mode, "implementation-navigation-", 26U) == 0)
        return TypeNavigationFixture(mode + 15U, 1);
    if (strncmp(mode, "hover-", 6U) == 0)
        return HoverFixture(mode);
    if (strncmp(mode, "formatting-", 11U) == 0)
        return FormattingFixture(mode);
    if (strncmp(mode, "range-formatting-", 17U) == 0)
        return RangeFormattingFixture(mode);
    if (strncmp(mode, "completion-", 11U) == 0)
        return CompletionFixture(mode);
    if (strncmp(mode, "lsp", 3U) == 0)
        return LanguageFixture(mode);
    if (strcmp(mode, "args") == 0)
    {
        for (int i = 2; i < argc; ++i)
            Hex(argv[i]);
        return 0;
    }
    if (strcmp(mode, "cwd") == 0)
    {
        char current[8192];
#ifdef _WIN32
        wchar_t wide[4096];
        DWORD count = GetCurrentDirectoryW(4096U, wide);
        if (count == 0U || count >= 4096U ||
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, current, (int)sizeof(current), NULL,
                                NULL) <= 0)
            return 3;
#else
        if (getcwd(current, sizeof(current)) == NULL)
            return 3;
#endif
        /* Normalise separators only for comparison; do not change the actual
         * current directory or reinterpret any argument as a path. */
        for (char *p = current; *p != '\0'; ++p)
            if (*p == '\\')
                *p = '/';
        Hex(current);
        return 0;
    }
    if (strcmp(mode, "environment") == 0)
    {
        const char *value = getenv("UMICOM_LANGUAGE_FIXTURE_VALUE");
        Hex(value == NULL ? "" : value);
        return 0;
    }
    if (strcmp(mode, "streams") == 0)
    {
        fputs("diagnostic\n", stderr);
        fflush(stderr);
        fputs("protocol\n", stdout);
        fflush(stdout);
        return 0;
    }
    if (strcmp(mode, "echo") == 0)
    {
        unsigned char bytes[32768];
        size_t count = fread(bytes, 1U, sizeof(bytes), stdin);
        if (count != sizeof(bytes))
            return 3;
        return fwrite(bytes, 1U, count, stdout) == count && fflush(stdout) == 0 ? 0 : 3;
    }
    if (strcmp(mode, "closed-input") == 0)
    {
#ifdef _WIN32
        if (_close(_fileno(stdin)) != 0)
            return 3;
#else
        if (close(STDIN_FILENO) != 0)
            return 3;
#endif
    }
    if (strcmp(mode, "wait") == 0 || strcmp(mode, "closed-input") == 0)
    {
        fputs("ready\n", stdout);
        fflush(stdout);
#ifdef _WIN32
        Sleep(60000U);
#else
        struct timespec delay = {60, 0};
        (void)nanosleep(&delay, NULL);
#endif
        return 0;
    }
    if (strcmp(mode, "exit-active") == 0)
        return 259;
    return 2;
}
#include "../native_process/utf8_entry.inc"
