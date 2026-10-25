/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/search_page.c
 *
 * PURPOSE:
 *   Keep search result identity and bounded page arithmetic in the shared platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/search_page.h"
#include <stdlib.h>
#include <string.h>

struct UmiFileSearchPage {
  UmiFileSearchSession *owner; /* Identity only; never dereferenced by readers. */
  UmiFileSearchPageSummary summary;
  UmiSearchMatch rows[];
};
static UmiStatus SearchPageReady(const UmiFileSearchSnapshot *state) {
  if (state->active || !state->ready || state->stale || state->requestId == 0U)
    return UMI_STATUS_BUSY;
  if (state->status != UMI_STATUS_OK)
    return state->status;
  if (state->stats.matches > UMI_FILE_SEARCH_MAX_RESULTS)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus UmiFileSearchPageValidate(const UmiFileSearchPage *page, UmiFileSearchSession *session) {
  if (page == NULL || session == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (page->owner != session)
    return UMI_STATUS_INVALID_STATE;
  UmiFileSearchSnapshot now;
  UmiStatus status = UmiFileSearchRead(session, &now);
  if (status != UMI_STATUS_OK)
    return status;
  status = SearchPageReady(&now);
  if (status != UMI_STATUS_OK)
    return status;
  const UmiFileSearchSnapshot *then = &page->summary.search;
  if (now.requestId != then->requestId || now.indexRevision != then->indexRevision ||
      now.stats.matches != then->stats.matches || now.caseSensitive != then->caseSensitive ||
      strcmp(now.query, then->query) != 0 || strcmp(now.root, then->root) != 0)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus UmiFileSearchPageCreate(UmiFileSearchSession *session, size_t offset, size_t limit,
                                  UmiFileSearchPage **out_page) {
  if (session == NULL || out_page == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (limit == 0U)
    limit = UMI_FILE_SEARCH_PAGE_DEFAULT;
  if (limit > UMI_FILE_SEARCH_PAGE_LIMIT)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiFileSearchSnapshot state;
  UmiStatus status = UmiFileSearchRead(session, &state);
  if (status != UMI_STATUS_OK)
    return status;
  status = SearchPageReady(&state);
  if (status != UMI_STATUS_OK)
    return status;
  /* Subtract before division: an arbitrary caller offset, including SIZE_MAX,
   * cannot overflow page arithmetic or allocate proportional to that offset. */
  size_t total = state.stats.matches;
  size_t last = total == 0U ? 0U : ((total - 1U) / limit) * limit;
  offset = offset > last ? last : (offset / limit) * limit;
  size_t count = total - offset;
  if (count > limit)
    count = limit;
  UmiFileSearchPage *page = calloc(1, sizeof(*page) + count * sizeof(UmiSearchMatch));
  if (page == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  page->owner = session;
  page->summary.search = state;
  page->summary.offset = offset;
  page->summary.count = count;
  page->summary.limit = limit;
  page->summary.page_index = total == 0U ? 0U : offset / limit + 1U;
  page->summary.page_count = total == 0U ? 0U : (total - 1U) / limit + 1U;
  page->summary.has_previous = offset != 0U;
  page->summary.has_next = count < total - offset;
  page->summary.previous_offset = offset == 0U ? 0U : offset - limit;
  page->summary.next_offset = page->summary.has_next ? offset + count : offset;
  status = UmiFileSearchFilterRead(session, state.requestId, &page->summary.filter);
  for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    status = UmiFileSearchMatchAt(session, state.requestId, offset + i, &page->rows[i]);
  /* A page is one coherent result set, never a collection of rows assembled
   * across a refreshed file index or a replacement search request. */
  if (status == UMI_STATUS_OK)
    status = UmiFileSearchPageValidate(page, session);
  if (status != UMI_STATUS_OK) {
    free(page);
    return status;
  }
  *out_page = page;
  return UMI_STATUS_OK;
}
void UmiFileSearchPageDestroy(UmiFileSearchPage *page) { free(page); }
UmiStatus UmiFileSearchPageRead(const UmiFileSearchPage *page,
                                UmiFileSearchPageSummary *out_summary) {
  if (page == NULL || out_summary == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_summary = page->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiFileSearchPageMatchAt(const UmiFileSearchPage *page, size_t position,
                                   UmiSearchMatch *out_match) {
  if (page == NULL || out_match == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (position >= page->summary.count)
    return UMI_STATUS_NOT_FOUND;
  *out_match = page->rows[position];
  return UMI_STATUS_OK;
}
