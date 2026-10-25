/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/authorized_form_records.h
 *
 * PURPOSE:
 *   Typed form persistence through the current record-access authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Typed form persistence through the current record-access authority. */
#ifndef UMICOM_DATA_AUTHORIZED_FORM_RECORDS_H
#define UMICOM_DATA_AUTHORIZED_FORM_RECORDS_H
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/data/authorized_records.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAuthorizedFormRecord {
  UmiRecordRevision record;
  UmiStructuredForm form;
  UmiRecordAccessReceipt access;
} UmiAuthorizedFormRecord;
/* Draft saves retain unanswered required fields. The caller owns submission
 * validation. Input forms are never marked saved; publish a saved marker only
 * after reconciling the returned storage revision with the still-current draft.
 * CREATE uses expected_record_revision=0. AMEND also requires READ_HEAD for the
 * same purpose: the previous document must still be the reviewed revision and
 * must decode under this exact schema. A schema migration is a separate action.
 * All inputs and the schema remain stable until return; the authorized-record
 * service's single trusted owner and dependency lifetimes remain required. */
UmiStatus umi_authorized_form_record_save(UmiAuthorizedRecords *records,
                                          const UmiRecordAccessRequest *request,
                                          const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form, const char *reason,
                                          uint64_t observed_at_nanoseconds,
                                          uint64_t expected_record_revision,
                                          UmiAuthorizedRecordRevision *out);
/* Revision zero selects the current head and requires READ_HEAD. A positive
 * revision requires READ_HISTORY. Stored bytes are decoded only after access
 * succeeds. Return record, typed answers and receipt together, or change none
 * of the output. Loaded answers are marked saved at their own edit revision. */
UmiStatus umi_authorized_form_record_load(UmiAuthorizedRecords *records,
                                          const UmiRecordAccessRequest *request,
                                          uint64_t record_revision,
                                          const UmiStructuredFormSchema *schema,
                                          UmiAuthorizedFormRecord *out);
UmiStatus umi_authorized_form_record_open_reviewed(UmiAuthorizedRecords *records,
                                                   const UmiRecordAccessRequest *request,
                                                   const UmiRecordRevision *reviewed,
                                                   const UmiStructuredFormSchema *schema,
                                                   UmiAuthorizedFormRecord *out);
#ifdef __cplusplus
}
#endif
#endif
