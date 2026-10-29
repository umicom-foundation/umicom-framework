/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_coverage_text_private.h
 *
 * PURPOSE:
 *   Share escaped row presentation between complete and filtered coverage views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_CTEST_COVERAGE_TEXT_PRIVATE_H
#define UMICOM_CTEST_COVERAGE_TEXT_PRIVATE_H
#include "umicom/testing/ctest_coverage.h"
#include <stdio.h>
#include <string.h>

typedef struct CoverageWriter {
    char *text;
    size_t used;
} CoverageWriter;

static inline void Append(CoverageWriter *writer, const char *text)
{
    size_t length = strlen(text);
    if (writer->text) memcpy(writer->text + writer->used, text, length);
    writer->used += length;
}
static inline void Text(CoverageWriter *writer, const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        /* Escape separators as well as control bytes. A copied page must not
         * let imported text look like another host/outcome line. */
        char escaped[5] = {(char)*p, '\0'};
        if (*p < 32U || *p == 127U || *p == '\\')
            (void)snprintf(escaped, sizeof escaped, "\\x%02X", (unsigned)*p);
        Append(writer, escaped);
    }
}
static inline void CoverageTextRow(CoverageWriter *writer, const UmiCtestCoverageRow *row) {
    char line[640];
        Append(writer, "\n[");
        Append(writer, UmiCtestCoverageOutcomeText(row->outcome));
        Append(writer, "] ");
        Text(writer, row->requirement.host);
        Append(writer, " / ");
        Text(writer, row->requirement.configuration);
        Append(writer, " : ");
        Text(writer, row->requirement.name);
        if (row->requirement.capability[0]) {
            Append(writer, "\n  Requires: ");
            Text(writer, row->requirement.capability);
        }
        if (row->observed_source_identity[0]) {
            Append(writer, "\n  Observed source: ");
            Text(writer, row->observed_source_identity);
        }
        if (row->has_observation) {
            (void)snprintf(line, sizeof line, "\n  Saved duration: %llu ms",
                (unsigned long long)row->observation.duration_ms);
            Append(writer, line);
            /* A result message is saved evidence, not an inferred cause.
             * Reuse escaped text for both CLI and native pages so a newline
             * in imported metadata cannot impersonate another result row. */
            Append(writer, "\n  Saved result message: ");
            if (row->observation.message[0] != '\0')
                Text(writer, row->observation.message);
            else
                Append(writer, "not recorded");
        } else {
            Append(writer, "\n  No matching observation for the requested source.");
        }
        Append(writer, "\n");
}
#endif
