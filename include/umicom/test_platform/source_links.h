/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/source_links.h
 * PURPOSE: Capture navigable source evidence from selected-test results and output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PLATFORM_SOURCE_LINKS_H
#define UMICOM_TEST_PLATFORM_SOURCE_LINKS_H
#include "umicom/test_platform/evidence.h"
#include "umicom/diagnostics/failure_parser.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TEST_SOURCE_LINK_LIMIT 256U
typedef enum UmiTestSourceLinkOrigin {
    UMI_TEST_SOURCE_RESULT_DETAILS = 1,
    UMI_TEST_SOURCE_RESULT_MESSAGE = 2,
    UMI_TEST_SOURCE_OUTPUT = 3
} UmiTestSourceLinkOrigin;
typedef struct UmiTestSourceLink {
    char item_id[128], session_id[128], record_id[128];
    UmiTestSourceLinkOrigin origin;
    UmiCompilerDiagnosticFields location;
} UmiTestSourceLink;
typedef struct UmiTestSourceLinksSummary {
    UmiTestEvidenceSummary evidence;
    size_t retained_links, recognised_lines, duplicate_lines, capacity_dropped, unparsed_lines;
} UmiTestSourceLinksSummary;
typedef struct UmiTestSourceLinks UmiTestSourceLinks;
/* Build an immutable, caller-owned collection from already captured evidence.
 * Results are visited newest first (details before message), then output in its
 * captured order. Each retained record is parsed independently; fragments from
 * separate output records are never joined into an invented source location.
 * Equal path/line/column within one session is one link; its first provenance
 * is retained. Other sessions remain separate. The summary discloses duplicates,
 * unparsed lines and capacity loss alongside original evidence retention.
 * No registry mutation, file access, process execution or callback occurs.
 * Failure sets *outLinks=NULL; the result survives evidence destruction. */
UmiStatus UmiTestSourceLinksCreate(const UmiTestEvidence *evidence, UmiTestSourceLinks **outLinks);
void UmiTestSourceLinksDestroy(UmiTestSourceLinks *links);
UmiStatus UmiTestSourceLinksGetSummary(const UmiTestSourceLinks *links, UmiTestSourceLinksSummary *outSummary);
/* Zero-based immutable read. Failed reads leave caller output unchanged. */
UmiStatus UmiTestSourceLinksAt(const UmiTestSourceLinks *links, size_t index, UmiTestSourceLink *outLink);
#ifdef __cplusplus
}
#endif
#endif
