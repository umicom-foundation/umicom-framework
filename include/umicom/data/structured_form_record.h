/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/structured_form_record.h
 * PURPOSE: Store versioned form answers through atomic record documents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_STRUCTURED_FORM_RECORD_H
#define UMICOM_DATA_STRUCTURED_FORM_RECORD_H
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/data/record_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormRecord {
  UmiRecordRevision record;
  UmiStructuredForm form;
} UmiStructuredFormRecord;
/* expected_record_revision refers to storage history, not the form editing
 * revision. Zero creates a new entity. Caller owns authorization and metadata.
 * Valid drafts may be saved with unanswered required fields; submission rules
 * are a separate explicit validation. The snapshot must fit the existing record
 * document limit. No truncation or partial append occurs. Output is unchanged
 * on failure; the input form is never marked saved implicitly. */
UmiStatus umi_structured_form_record_save(UmiDataServer *server, const UmiRecordAmendment *metadata,
                                          const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form,
                                          uint64_t expected_record_revision,
                                          UmiRecordRevision *out);
/* Load revision zero for latest, or a specific historical record revision.
 * The original matching schema is required. Content hashing and framing are
 * checked before publication; the loaded form is marked saved at its own edit
 * revision. Both outputs are returned together, or neither changes. */
UmiStatus umi_structured_form_record_load(UmiDataServer *server, const char *entity_id,
                                          uint64_t record_revision,
                                          const UmiStructuredFormSchema *schema,
                                          UmiStructuredFormRecord *out);
/* Open the exact record metadata selected from a catalogue. A newer head or
 * changed metadata refuses the operation instead of opening another version. */
UmiStatus umi_structured_form_record_open_reviewed(UmiDataServer *server,
                                                   const UmiRecordRevision *reviewed,
                                                   const UmiStructuredFormSchema *schema,
                                                   UmiStructuredFormRecord *out);
#ifdef __cplusplus
}
#endif
#endif
