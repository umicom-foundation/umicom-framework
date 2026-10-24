/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Verified definitions retain their exact trusted historical schema. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_SEARCH_READER_H
#define UMICOM_DATA_STRUCTURED_FORM_SEARCH_READER_H
#include "umicom/application/experience/structured_form_search_resolver.h"
#include "umicom/data/record_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormHistoricalSearch {
  UmiRecordRevision record;
  UmiStructuredFormResolvedSearch resolved;
} UmiStructuredFormHistoricalSearch;
/* Decode validates entity namespace, metadata and payload digest before resolving
 * the exact schema. It performs no I/O. Load accepts zero for latest or an exact
 * historical revision. Reviewed open refuses a changed head or edited metadata.
 * The catalogue and schemas are borrowed immutable data; keep them alive.
 * All outputs are unchanged on failure. Allocate large results on the heap. */
UmiStatus umi_structured_form_search_reader_decode(const UmiStructuredFormCatalogue *catalogue,
                                                   const UmiRecordDocument *document,
                                                   UmiStructuredFormHistoricalSearch *out);
UmiStatus umi_structured_form_search_reader_load(UmiDataServer *server,
                                                 const UmiStructuredFormCatalogue *catalogue,
                                                 const char *entity_id, uint64_t revision,
                                                 UmiStructuredFormHistoricalSearch *out);
UmiStatus umi_structured_form_search_reader_open_reviewed(
    UmiDataServer *server, const UmiStructuredFormCatalogue *catalogue,
    const UmiRecordRevision *reviewed, UmiStructuredFormHistoricalSearch *out);
#ifdef __cplusplus
}
#endif
#endif
