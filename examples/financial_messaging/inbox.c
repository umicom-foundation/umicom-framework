/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/financial_messaging/inbox.c
 *
 * PURPOSE:
 *   Demonstrate repeatable local journal receipts using the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/sha256.h"
#include "umicom/financial_messaging/inbox_store.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
  if (argc != 2) {
    fputs("Usage: financial-inbox-example <disposable-synthetic-database>\n", stderr);
    return 2;
  }
  UmiUfmpEnvelope envelope = {0};
  envelope.revision = UMI_UFMP_LOGICAL_REVISION;
  envelope.type = UMI_UFMP_PAYMENT_INITIATE;
  envelope.format = UMI_UFMP_PAYLOAD_CONTROL_CBOR;
  strcpy(envelope.sender, "synthetic.Bank.B");
  strcpy(envelope.recipient, "synthetic.Bank.A");
  strcpy(envelope.message_id, "demo.message.1");
  strcpy(envelope.conversation_id, "demo.conversation.1");
  strcpy(envelope.idempotency_key, "demo.operation.1");
  strcpy(envelope.schema, "synthetic.scalar.1");
  envelope.created_ms = 1000U;
  envelope.expires_ms = 2000U;
  const unsigned char body[] = {0x01U}; /* CBOR unsigned integer one. */
  envelope.payload_bytes = sizeof body;
  UmiSha256 digest;
  UmiSha256Init(&digest);
  UmiStatus status = UmiSha256Update(&digest, body, sizeof body);
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&digest, envelope.content_digest);
  UmiDataServer *server = NULL;
  if (status == UMI_STATUS_OK)
    status = umi_data_server_create_sqlite(argv[1], &server);
  UmiUfmpInboxReceipt receipt;
  UmiUfmpEnvelopePolicy policy = {1100U, 50U, 1000U, 1024U, UMI_UFMP_LOGICAL_REVISION};
  if (status == UMI_STATUS_OK)
    status = UmiUfmpInboxReceive(server, &envelope, &policy, body, sizeof body, &receipt);
  if (status == UMI_STATUS_OK)
    printf("SYNTHETIC LOCAL JOURNAL ONLY\nReceipt: %s\nReplay classification: %d\n"
           "No funds moved; no network authentication or signature was exercised.\n",
           receipt.receipt_id, (int)receipt.match);
  else
    fprintf(stderr, "Local inbox failed: %s\n", umi_status_text(status));
  umi_data_server_destroy(server);
  return status == UMI_STATUS_OK ? 0 : 1;
}
