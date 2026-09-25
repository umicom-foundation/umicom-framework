/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/audit.c
 *
 * PURPOSE:
 *   Audit public include guards and flexible attribution through a shared C engine.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct GuardOwner { char *guard; const char *path; } GuardOwner;
static int pathOrder(const void *a, const void *b)
{
    const char *left = *(const char *const *)a, *right = *(const char *const *)b;
#if defined(_WIN32)
    return _stricmp(left, right);
#else
    return strcmp(left, right);
#endif
}
static int guardOrder(const void *a, const void *b)
{
    const GuardOwner *left = a, *right = b;
    int result = strcmp(left->guard, right->guard);
    return result == 0 ? strcmp(left->path, right->path) : result;
}

/* The Notes CTest tutorial may create notes_tests.c as a local exercise file
 * beneath its example directory. It is not part of the maintained Framework
 * source catalogue on main, and rewriting an unknown local exercise would be
 * destructive. Keep the file visible to the scan count but exempt only this
 * exact local exercise path from repository metadata enforcement. */
static bool IsLocalNotesExercise(const char *path)
{
    static const char suffix[] = "examples/testing/notes_project/notes_tests.c";
    size_t pathLength;
    size_t suffixLength;
    size_t start;

    if (path == NULL) return false;
    pathLength = strlen(path);
    suffixLength = sizeof(suffix) - 1U;
    if (pathLength < suffixLength) return false;
    start = pathLength - suffixLength;
    for (size_t index = 0U; index < suffixLength; ++index) {
        char actual = path[start + index];
        if (actual == '\\') actual = '/';
        if (actual != suffix[index]) return false;
    }
    return true;
}

int UmiSourceContractAudit(const char *const *roots, size_t count,
    bool public_headers, UmiSourceContractReport *report)
{
    if (roots == NULL || count == 0U || report == NULL) return 2;
    ScFiles files = {0};
    GuardOwner *guards = NULL; size_t guardCount = 0U;
    size_t before = report->findings, inputBefore = report->input_errors;
    for (size_t i = 0U; i < count; ++i)
        if (ScCollect(roots[i], public_headers, &files) != 0)
            ScFinding(report, roots[i], 1U, "input.directory", "Missing, unreadable or unsupported directory", true);
    if (files.count == 0U) {
        ScFinding(report, roots[0], 1U, "input.empty", "No matching source files were found", true);
        ScFilesFree(&files); return 2;
    }
    qsort(files.items, files.count, sizeof(*files.items), pathOrder);
    if (public_headers) {
        guards = calloc(files.count, sizeof(*guards));
        if (guards == NULL) { ScFilesFree(&files); return 2; }
    }
    for (size_t i = 0U; i < files.count; ++i) {
        if (i != 0U && pathOrder(&files.items[i - 1U], &files.items[i]) == 0) continue;
        const char *path = files.items[i];
        char *source = ScRead(path);
        if (source == NULL) { ScFinding(report, path, 1U, "input.read", "Cannot read bounded text source (or embedded NUL found)", true); continue; }
        ++report->files_checked;
        if (!IsLocalNotesExercise(path)) {
            (void)ScMetadata(source, strlen(source), path, report);
        }
        if (public_headers) {
            char *guard = NULL;
            int result = ScGuard(source, path, &guard, report);
            if (result == 2) ScFinding(report, path, 1U, "input.guard", "Cannot inspect header guard", true);
            if (guard != NULL) guards[guardCount++] = (GuardOwner){guard, path};
        }
        free(source);
    }
    if (guardCount > 1U) {
        qsort(guards, guardCount, sizeof(*guards), guardOrder);
        for (size_t i = 1U; i < guardCount; ++i) {
            if (strcmp(guards[i - 1U].guard, guards[i].guard) == 0) {
                ScFinding(report, guards[i].path, 1U, "header.duplicate-guard", guards[i].guard, false);
                if (report->diagnostic != NULL) report->diagnostic(report->context, guards[i - 1U].path, 1U, "header.previous-owner", guards[i].guard);
            }
        }
    }
    for (size_t i = 0U; i < guardCount; ++i) free(guards[i].guard);
    free(guards); ScFilesFree(&files);
    return report->input_errors != inputBefore ? 2 : report->findings != before ? 1 : 0;
}
