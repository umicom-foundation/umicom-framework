/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Internal transaction composition; caller must begin and commit/rollback. */
#ifndef UMICOM_DATA_RECORD_DOCUMENT_INTERNAL_H
#define UMICOM_DATA_RECORD_DOCUMENT_INTERNAL_H
#include "umicom/data/record_document.h"
UmiStatus umi_record_document_read_in_transaction(UmiDataServer *server, const char *entity_id,
                                                  uint64_t revision, UmiRecordDocument *out);
UmiStatus umi_record_document_stage_in_transaction(UmiDataServer *server,
                                                   const UmiRecordDocument *document);
#endif
