/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/identity.c
 *
 * PURPOSE:
 *   Match every native window to shared identity through nested GTK casts.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Strip only transparent GTK_WINDOW/GTK_WIDGET wrappers and grouping. The
 * wrapped expression retains its member chain; arbitrary expressions are not
 * silently equated to a window. */
static void unwrap(const ScText *text, size_t *first, size_t *last)
{
    for (size_t depth = 0U; depth < SC_MAX_DEPTH && *first < *last; ++depth) {
        if (ScTokenEquals(text, *first, "(") && ScMatching(text, *first) + 1U == *last) { ++*first; --*last; continue; }
        if ((ScTokenEquals(text, *first, "GTK_WINDOW") || ScTokenEquals(text, *first, "GTK_WIDGET")) &&
            ScTokenEquals(text, *first + 1U, "(") && ScMatching(text, *first + 1U) + 1U == *last) {
            *first += 2U; --*last; continue;
        }
        break;
    }
}
static bool memberExpression(const ScText *text, size_t first, size_t last)
{
    if (first >= last || !ScIdentifier(text, first)) return false;
    size_t i = first + 1U;
    while (i < last) {
        if (ScTokenEquals(text, i, ".")) ++i;
        else if (ScTokenEquals(text, i, "-") && ScTokenEquals(text, i + 1U, ">")) i += 2U;
        else return false;
        if (i >= last || !ScIdentifier(text, i)) return false;
        ++i;
    }
    return true;
}
int UmiSourceContractWindowIdentity(const char *source, const char *path,
    bool studio_fragment, UmiSourceContractReport *report)
{
    if (source == NULL || path == NULL || report == NULL) return 2;
    ScText text; size_t before = report->findings;
    if (ScLex(&text, source, false) != 0) {
        ScFinding(report, path, 1U, "identity.parse", "Malformed source", true); return 2;
    }
    char *identities[256] = {0}; size_t identityCount = 0U, windowCount = 0U;
    for (size_t i = 0U; i + 1U < text.count; ++i) {
        if (!ScTokenEquals(&text, i, "umi_gtk4_ws_apply_window_identity") || !ScTokenEquals(&text, i + 1U, "(")) continue;
        size_t end = ScMatching(&text, i + 1U), first = i + 2U;
        if (end == SIZE_MAX) { ScFinding(report, path, ScLine(&text, i), "identity.arguments", "Unbalanced identity call", true); continue; }
        size_t last = end;
        unwrap(&text, &first, &last);
        if (!memberExpression(&text, first, last)) continue;
        if (identityCount == 256U) { ScFinding(report, path, ScLine(&text, i), "identity.capacity", "Too many identity calls", true); break; }
        identities[identityCount] = ScCompactRange(&text, first, last);
        if (identities[identityCount] == NULL) { ScFinding(report, path, ScLine(&text, i), "identity.memory", "Allocation failed", true); break; }
        ++identityCount;
    }
    if (identityCount == 0U) ScFinding(report, path, 1U, "identity.missing", "No shared window identity request", false);
    if (studio_fragment) {
        bool found = false;
        for (size_t i = 0U; i < identityCount; ++i)
            if (strcmp(identities[i], "runtime->owner->window") == 0) found = true;
        ++report->checks;
        if (!found) ScFinding(report, path, 1U, "identity.studio", "Studio's borrowed main window needs shared identity", false);
    } else {
        /* Assignment targets are compared, not the number of calls. Independent
         * splash and main windows each need their own identity. */
        for (size_t i = 1U; i + 2U < text.count; ++i) {
            if (!ScTokenEquals(&text, i, "=")) continue;
            size_t next = i + 1U;
            while (ScTokenEquals(&text, next, "GTK_WINDOW") && ScTokenEquals(&text, next + 1U, "(")) next += 2U;
            if ((!ScTokenEquals(&text, next, "gtk_window_new") && !ScTokenEquals(&text, next, "gtk_application_window_new")) || !ScTokenEquals(&text, next + 1U, "(")) continue;
            size_t first = i - 1U;
            while (first >= 2U) {
                if (ScTokenEquals(&text, first - 1U, ".") && ScIdentifier(&text, first - 2U)) first -= 2U;
                else if (first >= 3U && ScTokenEquals(&text, first - 1U, ">") && ScTokenEquals(&text, first - 2U, "-") && ScIdentifier(&text, first - 3U)) first -= 3U;
                else break;
            }
            if (!memberExpression(&text, first, i)) continue;
            ++windowCount; ++report->checks;
            char *window = ScCompactRange(&text, first, i);
            bool found = false;
            if (window == NULL) { ScFinding(report, path, ScLine(&text, i), "identity.memory", "Allocation failed", true); break; }
            for (size_t n = 0U; n < identityCount; ++n) if (strcmp(window, identities[n]) == 0) found = true;
            if (!found) ScFinding(report, path, ScLine(&text, i), "identity.window", window, false);
            free(window);
        }
        if (windowCount == 0U) ScFinding(report, path, 1U, "identity.construction", "Native window construction was not found", false);
    }
    for (size_t i = 0U; i < identityCount; ++i) free(identities[i]);
    ScTextFree(&text);
    return report->findings == before ? 0 : 1;
}
