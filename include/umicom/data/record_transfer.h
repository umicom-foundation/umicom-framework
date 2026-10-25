/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_transfer.h
 *
 * PURPOSE:
 *   Import provenance is separate from the original record's authorship.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Import provenance is separate from the original record's authorship. */
#ifndef UMICOM_DATA_RECORD_TRANSFER_H
#define UMICOM_DATA_RECORD_TRANSFER_H
#include "umicom/data/record_bundle.h"
#include "umicom/data/record_draft.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_TRANSFER_ID_CAPACITY 41U
/* The caller assigns a unique ASCII letters/digits/_- receipt ID. Attribution
 * describes this import, not the original author. It is supplied by the host,
 * never inferred from an untrusted file. Ordinary labels are not authentication. */
typedef struct UmiRecordTransferRequest {
  char receipt_id[UMI_RECORD_TRANSFER_ID_CAPACITY];
  UmiRecordDraftAttribution attribution;
} UmiRecordTransferRequest;
typedef struct UmiRecordTransferReceipt {
  UmiRecordRevision receipt;
  UmiRecordRevision imported_head;
  char bundle_sha256[65];
} UmiRecordTransferReceipt;
/* Validate a caller-assigned receipt identifier without reading storage. */
UmiStatus umi_record_transfer_id_validate(const char *receipt_id);
/* Validate bounded import attribution and receipt identity without storage. */
UmiStatus umi_record_transfer_request_validate(const UmiRecordTransferRequest *request);
/* Restore complete reviewed history and its immutable import receipt in ONE
 * owned transaction. The supplied checksum must equal the encoded bundle's
 * checksum. Both record identities must be unoccupied, including orphan rows.
 * Nested transactions are BUSY. Any error leaves output unchanged; rollback
 * failure is IO_ERROR. No event is written for a rejected attempt.
 * Reserve the transfer.receipt. record namespace for this service. Bundles
 * whose original entity uses that namespace are refused. No automatic retry,
 * deduplication, overwriting, authentication or external audit submission.
 * The caller authorizes the destination and serializes the entire operation. */
UmiStatus umi_record_transfer_restore(UmiDataServer *server, const UmiRecordBundle *bundle,
                                      const char *reviewed_sha256,
                                      const UmiRecordTransferRequest *request,
                                      UmiRecordTransferReceipt *out);
/* Decode a copied receipt document. The complete record metadata/content digest,
 * fixed receipt namespace and canonical original metadata are checked before
 * output is replaced. This performs no storage access or authorization. */
UmiStatus umi_record_transfer_receipt_decode(const UmiRecordDocument *document,
                                             UmiRecordTransferReceipt *out);
/* Check that copied receipt fields reconstruct the canonical receipt payload.
 * This detects inconsistent copies; it neither reads the store nor authenticates
 * the original event. */
UmiStatus umi_record_transfer_receipt_validate(const UmiRecordTransferReceipt *receipt);
/* Read revision one of a receipt by its caller-assigned ID. A receipt with an
 * amended head is refused. Verify also reads ALL originally imported revisions
 * and checks their complete bundle checksum in that same owned transaction.
 * Later additions to the imported record are allowed and are not part of this
 * historical receipt. This detects inconsistency, not malicious replacement
 * by someone able to rewrite the entire store. Outputs change only on success. */
UmiStatus umi_record_transfer_receipt_load(UmiDataServer *server, const char *receipt_id,
                                           UmiRecordTransferReceipt *out);
UmiStatus umi_record_transfer_receipt_verify(UmiDataServer *server, const char *receipt_id,
                                             UmiRecordTransferReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
