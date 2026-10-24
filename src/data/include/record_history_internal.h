/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Internal composition only: callers must own and complete a Data Server
 * transaction. Staged revisions must never be published before commit. */
#ifndef UMICOM_DATA_RECORD_HISTORY_INTERNAL_H
#define UMICOM_DATA_RECORD_HISTORY_INTERNAL_H
#include "umicom/data/record_history.h"
UmiStatus umi_record_history_key(const char *entity, uint64_t revision,
                                 char *key, size_t capacity);
UmiStatus umi_record_history_stage(UmiDataServer *server,
                                   const UmiRecordAmendment *amendment,
                                   uint64_t expected_revision,
                                   UmiRecordRevision *output);
#endif
