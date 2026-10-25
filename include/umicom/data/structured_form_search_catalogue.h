/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/structured_form_search_catalogue.h
 *
 * PURPOSE:
 *   Discover verified saved queries without exposing raw database key encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Discover verified saved queries without exposing raw database key encoding. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_SEARCH_CATALOGUE_H
#define UMICOM_DATA_STRUCTURED_FORM_SEARCH_CATALOGUE_H
#include "umicom/data/record_catalogue.h"
#include "umicom/data/structured_form_search_reader.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormSearchQuery {
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t schema_version;
  char entity_prefix[64]; /* Exact searched-record namespace, never a permission. */
  char text[160];         /* Case-sensitive substring of preset title or entity key. */
  char after_entity[64];
  char expected_snapshot[65];
  size_t limit;
} UmiStructuredFormSearchQuery;
typedef struct UmiStructuredFormSearchRow {
  UmiRecordRevision record;
  char title[160];
  size_t term_count;
} UmiStructuredFormSearchRow;
typedef struct UmiStructuredFormSearchPage {
  char snapshot[65], next_after_entity[64];
  size_t count, matching_count, definition_count;
  UmiStructuredFormSearchRow rows[UMI_RECORD_CATALOGUE_PAGE_CAPACITY];
} UmiStructuredFormSearchPage;
/* Scan form.search. heads, verify each document and decode its exact registered
 * schema before filtering by form/version, exact record namespace and title/key.
 * Known other forms are excluded. Unknown versions or malformed definitions
 * fail the whole scan, including definitions that would otherwise be excluded.
 * There is no silent partial success or untrusted schema fallback.
 *
 * Empty cursor/snapshot starts a new search. Continue using the returned cursor
 * and snapshot with unchanged filters. A missing cursor or changed definition
 * head refuses the page. Every scan rechecks the initial head snapshot after
 * individual document reads; this is not a global transaction across all reads.
 * At most 256 preset heads and 32768 storage rows are scanned by the underlying
 * catalogue. Digests detect changes; they are not authorization or signatures.
 * Results sort by entity bytes. limit is 1..16. Output is unchanged on failure.
 * Use the reviewed reader before acting on a selected row. */
UmiStatus umi_structured_form_search_catalogue_page(UmiDataServer *server,
                                                    const UmiStructuredFormCatalogue *catalogue,
                                                    const UmiStructuredFormSearchQuery *query,
                                                    UmiStructuredFormSearchPage *out);
#ifdef __cplusplus
}
#endif
#endif
