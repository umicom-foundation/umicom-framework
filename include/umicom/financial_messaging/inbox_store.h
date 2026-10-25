/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/inbox_store.h
 *
 * PURPOSE:
 *   Persist bounded financial message receipts through Data Server ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_INBOX_STORE_H
#define UMICOM_FINANCIAL_MESSAGING_INBOX_STORE_H
#include "umicom/data/data_server.h"
#include "umicom/financial_messaging/replay.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UFMP_INBOX_PAYLOAD_CAPACITY 1024U
#define UMI_UFMP_RECEIPT_CAPACITY 65U
typedef struct UmiUfmpInboxReceipt {
  char receipt_id[UMI_UFMP_RECEIPT_CAPACITY];
  char original_message_id[UMI_UFMP_ID_CAPACITY];
  UmiUfmpReplayMatch match; /* NO_MATCH means newly recorded. */
  UmiDataServerBackend backend;
} UmiUfmpInboxReceipt;
typedef struct UmiUfmpInboxRecord {
  UmiUfmpEnvelope envelope;
  unsigned char payload[UMI_UFMP_INBOX_PAYLOAD_CAPACITY];
  size_t payload_length;
} UmiUfmpInboxRecord;
/* This local admission journal is for ALREADY authenticated, authorized and
 * schema-checked messages. It checks logical policy and body integrity again.
 * It does not verify COSE, certificates or participant permissions, execute a
 * payment, or send an acknowledgement. A successful call means only that the
 * exact body, envelope and message/operation indexes committed together.
 * The memory backend is useful for tests and is never crash-durable.
 * SQLite durability follows the Data Server and storage configuration.
 *
 * This bounded foundation accepts at most 1024 payload bytes. Keys under
 * "ufmp.inbox.v1/" belong exclusively to this service; never modify them
 * through another writer. One Data Server database is one deduplication scope.
 * The operation key is scoped by exact sender ID, including across recipients.
 * Identical retries return the original receipt without a second record.
 * A changed use of an existing key/ID returns ALREADY_EXISTS.
 *
 * Owns begin/commit/rollback on the calling thread. An existing transaction
 * returns BUSY; no nested transaction or rollback of another owner occurs.
 * Output is unchanged on failure. Never delete deduplication records merely
 * because their original envelope expired. Retention needs a separate policy.
 * Expired incoming retries are rejected; send a fresh delivery envelope.
 */
UmiStatus UmiUfmpInboxReceive(UmiDataServer *server, const UmiUfmpEnvelope *envelope,
                              const UmiUfmpEnvelopePolicy *policy, const void *payload,
                              size_t length, UmiUfmpInboxReceipt *out_receipt);
/* Load the original record using a 64-character lowercase hex receipt.
 * This does not reapply expiry policy to historical evidence. Stored field,
 * payload and identity integrity are checked; corruption is PARSE_ERROR.
 * Output is unchanged on failure. Caller-owned objects must not overlap.
 * The digest detects accidental corruption, not a malicious database writer. */
UmiStatus UmiUfmpInboxLoad(const UmiDataServer *server, const char *receipt_id,
                           UmiUfmpInboxRecord *out_record);
#ifdef __cplusplus
}
#endif
#endif
