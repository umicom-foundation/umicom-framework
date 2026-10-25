/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/include/record_transfer_internal.h
 *
 * PURPOSE:
 *   Receipt framing is shared by the transaction owner and checked reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Receipt framing is shared by the transaction owner and checked reader. */
#ifndef UMICOM_DATA_RECORD_TRANSFER_INTERNAL_H
#define UMICOM_DATA_RECORD_TRANSFER_INTERNAL_H
#include "umicom/data/record_transfer.h"
UmiStatus umi_record_transfer_receipt_identity(const char *id, char out[64]);
UmiStatus umi_record_transfer_receipt_document(const UmiRecordRevision *imported,
                                               const char digest[65],
                                               const UmiRecordTransferRequest *request,
                                               UmiRecordDocument *out);
#endif
