/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Search verified typed answers using registered semantic schema versions. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_QUERY_H
#define UMICOM_DATA_STRUCTURED_FORM_QUERY_H
#include "umicom/application/experience/structured_form_filter.h"
#include "umicom/data/record_catalogue.h"
#include "umicom/data/structured_form_reader.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormQuery {
  char entity_prefix[64], metadata_text[160];
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t schema_version;
  UmiStructuredFormFilter filter;
  char after_entity[64], expected_snapshot[65];
  size_t limit;
} UmiStructuredFormQuery;
typedef struct UmiStructuredFormQueryPage {
  char snapshot[65], next_after_entity[64];
  size_t count, matching_count, metadata_count, namespace_count;
  UmiRecordRevision rows[UMI_RECORD_CATALOGUE_PAGE_CAPACITY];
} UmiStructuredFormQueryPage;
/* Search one explicit nonempty namespace and one registered form ID/version.
 * Metadata text uses the existing catalogue's case-sensitive search. Every
 * selected metadata row is opened and integrity checked with its registered
 * original schema; other known form versions are excluded from answer matches.
 * Unknown schemas, metadata-only rows or damaged selected records fail the
 * whole request instead of producing a misleading partial result.
 *
 * The service uses bounded catalogue/record reads and then rechecks the initial
 * head snapshot. It does not hold one database transaction across the scan.
 * A changed head anywhere in the namespace invalidates the result. This is
 * optimistic consistency under the record append contract, not authentication
 * against arbitrary raw storage replacement. No retry hides concurrent writes.
 * Limits are inherited from the record catalogue (256 heads, 32768 storage
 * rows per scan, 16 result rows). Run on a storage worker for larger stores.
 *
 * First page: empty after_entity and expected_snapshot. Continue with the
 * returned cursor/digest and identical filters. A cursor must be a matching
 * row. Different filters or source heads refuse continuation. Limit may change.
 * Results retain exact reviewed metadata for reader_open_reviewed; no implicit
 * schema upgrade or draft replacement occurs. Output unchanged on failure.
 * Authorization and store/catalogue lifetimes remain the caller's responsibility. */
UmiStatus umi_structured_form_query_page(UmiDataServer *server,
                                         const UmiStructuredFormCatalogue *catalogue,
                                         const UmiStructuredFormQuery *query,
                                         UmiStructuredFormQueryPage *out);
#ifdef __cplusplus
}
#endif
#endif
