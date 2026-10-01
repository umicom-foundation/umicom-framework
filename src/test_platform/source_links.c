/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/source_links.c
 * PURPOSE: Derive bounded source links while retaining test/run provenance and loss evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/source_links.h"
#include <stdlib.h>
#include <string.h>
struct UmiTestSourceLinks {
    UmiTestSourceLinksSummary summary;
    UmiTestSourceLink links[UMI_TEST_SOURCE_LINK_LIMIT];
};
static int SameLocation(const UmiTestSourceLink *left, const UmiTestSourceLink *right)
{
    return strcmp(left->session_id, right->session_id) == 0 &&
        strcmp(left->location.path, right->location.path) == 0 &&
        left->location.line == right->location.line && left->location.column == right->location.column;
}
static UmiStatus AddText(UmiTestSourceLinks *links, const char *text, const char *session,
    const char *record, UmiTestSourceLinkOrigin origin)
{
    const char *cursor = text;
    while (*cursor != '\0') {
        const char *newline = strchr(cursor, '\n');
        size_t length = newline != NULL ? (size_t)(newline - cursor) : strlen(cursor);
        char line[8192]; UmiTestSourceLink candidate = {0};
        UmiStatus status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (length < sizeof(line)) {
            memcpy(line, cursor, length); line[length] = '\0';
            status = UmiTestFailureParseText(line, &candidate.location);
        }
        if (status == UMI_STATUS_OK) {
            ++links->summary.recognised_lines;
            strcpy(candidate.item_id, links->summary.evidence.item_id);
            strcpy(candidate.session_id, session); strcpy(candidate.record_id, record); candidate.origin = origin;
            int duplicate = 0;
            for (size_t i = 0U; i < links->summary.retained_links; ++i)
                if (SameLocation(&candidate, &links->links[i])) { duplicate = 1; break; }
            if (duplicate) ++links->summary.duplicate_lines;
            else if (links->summary.retained_links == UMI_TEST_SOURCE_LINK_LIMIT) ++links->summary.capacity_dropped;
            else links->links[links->summary.retained_links++] = candidate;
        } else if (status == UMI_STATUS_CAPACITY_EXCEEDED) ++links->summary.capacity_dropped;
        else if (status == UMI_STATUS_NOT_FOUND) ++links->summary.unparsed_lines;
        else return status;
        if (newline == NULL) break;
        cursor = newline + 1;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiTestSourceLinksCreate(const UmiTestEvidence *evidence, UmiTestSourceLinks **outLinks)
{
    if (outLinks == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outLinks = NULL;
    if (evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestSourceLinks *links = calloc(1U, sizeof(*links));
    if (links == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiTestEvidenceGetSummary(evidence, &links->summary.evidence);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < links->summary.evidence.retained_results; ++i) {
        UmiTestPlatformResultSnapshot result;
        status = UmiTestEvidenceResultAt(evidence, i, &result);
        if (status == UMI_STATUS_OK) status = AddText(links, result.failure_details, result.session_id, result.id, UMI_TEST_SOURCE_RESULT_DETAILS);
        if (status == UMI_STATUS_OK) status = AddText(links, result.message, result.session_id, result.id, UMI_TEST_SOURCE_RESULT_MESSAGE);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < links->summary.evidence.retained_output; ++i) {
        UmiTestPlatformOutputSnapshot output;
        status = UmiTestEvidenceOutputAt(evidence, i, &output);
        if (status == UMI_STATUS_OK) status = AddText(links, output.text, output.session_id, output.id, UMI_TEST_SOURCE_OUTPUT);
    }
    if (status != UMI_STATUS_OK) { free(links); return status; }
    *outLinks = links; return UMI_STATUS_OK;
}
void UmiTestSourceLinksDestroy(UmiTestSourceLinks *links) { free(links); }
UmiStatus UmiTestSourceLinksGetSummary(const UmiTestSourceLinks *links, UmiTestSourceLinksSummary *outSummary)
{
    if (links == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = links->summary; return UMI_STATUS_OK;
}
UmiStatus UmiTestSourceLinksAt(const UmiTestSourceLinks *links, size_t index, UmiTestSourceLink *outLink)
{
    if (links == NULL || outLink == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= links->summary.retained_links) return UMI_STATUS_NOT_FOUND;
    *outLink = links->links[index]; return UMI_STATUS_OK;
}
