/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Resolve immutable historical schemas after checking stored document integrity. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_READER_H
#define UMICOM_DATA_STRUCTURED_FORM_READER_H
#include "umicom/application/experience/structured_form_catalogue.h"
#include "umicom/data/record_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormHistorical {
  UmiRecordRevision record;
  UmiStructuredFormResolved resolved;
} UmiStructuredFormHistorical;
/* Verify a supplied document before full decoding with its exact trusted schema.
 * No I/O occurs. The resolved schema is borrowed from the caller's catalogue.
 * All outputs are unchanged on failure; allocate the large output on the heap. */
UmiStatus umi_structured_form_reader_decode(const UmiStructuredFormCatalogue *catalogue,
                                            const UmiRecordDocument *document,
                                            UmiStructuredFormHistorical *out);
/* Zero selects the current head. Other revisions select immutable history.
 * No upgrade occurs; the returned form is marked saved at its own edit revision. */
UmiStatus umi_structured_form_reader_load(UmiDataServer *server,
                                          const UmiStructuredFormCatalogue *catalogue,
                                          const char *entity_id, uint64_t record_revision,
                                          UmiStructuredFormHistorical *out);
/* A current catalogue row must still describe the current head. This refuses
 * stale selection rather than silently opening a newer revision. */
UmiStatus umi_structured_form_reader_open_reviewed(UmiDataServer *server,
                                                   const UmiStructuredFormCatalogue *catalogue,
                                                   const UmiRecordRevision *reviewed,
                                                   UmiStructuredFormHistorical *out);
#ifdef __cplusplus
}
#endif
#endif
