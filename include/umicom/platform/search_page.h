/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search_page.h
 *
 * PURPOSE:
 *   Capture bounded immutable pages from completed saved-file search requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_SEARCH_PAGE_H
#define UMICOM_PLATFORM_SEARCH_PAGE_H
#include "umicom/platform/search_session.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FILE_SEARCH_PAGE_DEFAULT 64U
#define UMI_FILE_SEARCH_PAGE_LIMIT 128U
typedef struct UmiFileSearchPage UmiFileSearchPage;
typedef struct UmiFileSearchPageSummary {
  UmiFileSearchSnapshot search;
  UmiSearchPathFilter filter;
  size_t offset, count, limit, page_index, page_count;
  size_t previous_offset, next_offset;
  int has_previous, has_next;
} UmiFileSearchPageSummary;
/* Capture on the search session's owner thread. Only successful, non-stale,
 * completed requests are admitted. Zero limit means 64; a limit above 128 is
 * rejected. Offset is aligned to the chosen limit and clamped to the last page.
 * All output is unchanged on failure. The page owns copied rows and metadata,
 * so read-only inspection remains valid after the session is destroyed.
 * No file reads, waiting, callbacks or UI notifications occur here. */
UmiStatus UmiFileSearchPageCreate(UmiFileSearchSession *session, size_t offset, size_t limit,
                                  UmiFileSearchPage **out_page);
void UmiFileSearchPageDestroy(UmiFileSearchPage *page);
UmiStatus UmiFileSearchPageRead(const UmiFileSearchPage *page,
                                UmiFileSearchPageSummary *out_summary);
/* Position is relative to this page. Convert it through summary.offset only
 * after validating the page against the original live session. */
UmiStatus UmiFileSearchPageMatchAt(const UmiFileSearchPage *page, size_t position,
                                   UmiSearchMatch *out_match);
/* The original session must remain alive while validating or activating a row.
 * This is a result identity check, not proof that disk contents are unchanged.
 * Opening source still needs the existing literal-location/draft checks. */
UmiStatus UmiFileSearchPageValidate(const UmiFileSearchPage *page, UmiFileSearchSession *session);
#ifdef __cplusplus
}
#endif
#endif
