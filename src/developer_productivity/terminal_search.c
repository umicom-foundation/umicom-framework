/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_search.c
 *
 * PURPOSE:
 *   Implement bounded terminal-transcript search.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_search.h"

#include <ctype.h>
#include <string.h>

/* Provide the contains folded operation used by this module and its client applications. */
static int contains_folded(const char *text, const char *query)
{
    const char *start;

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = query;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) ==
               tolower((unsigned char)*b)) {
            ++a;
            ++b;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*b == '\0') return 1;
    }

    return query[0] == '\0';
}

/*
 * Provide the developer terminal search operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_terminal_search(
    const UmiTerminalTranscript *transcript,
    const char *query,
    int case_sensitive,
    UmiDeveloperTerminalSearchMatch *out_matches,
    size_t capacity,
    size_t *out_count)
{
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transcript == NULL || query == NULL ||
        out_matches == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_terminal_transcript_count(transcript) &&
         used < capacity;
         ++index) {
        UmiTerminalTranscriptLine line;
        int matched;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_terminal_transcript_at(
                transcript, index, &line) != UMI_STATUS_OK) {
            continue;
        }

        matched = case_sensitive
            ? strstr(line.text, query) != NULL
            : contains_folded(line.text, query);

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!matched) continue;

        out_matches[used].transcript_index = index;
        out_matches[used].sequence = line.sequence;
        (void)strncpy(
            out_matches[used].text,
            line.text,
            sizeof(out_matches[used].text) - 1U);
        out_matches[used].text[sizeof(out_matches[used].text) - 1U] = '\0';
        used += 1U;
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
