/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/types.h
 *
 * PURPOSE:
 *   Define bounded logical UFMP message identities for offline validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_TYPES_H
#define UMICOM_FINANCIAL_MESSAGING_TYPES_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UFMP_ID_CAPACITY 65U
#define UMI_UFMP_SCHEMA_CAPACITY 97U
#define UMI_UFMP_DIGEST_BYTES 32U
/* This is a logical draft contract, not a frozen CBOR/COSE wire profile. */
#define UMI_UFMP_LOGICAL_REVISION 1U
typedef enum UmiUfmpMessageType {
  UMI_UFMP_NETWORK_HELLO = 1,
  UMI_UFMP_NETWORK_CAPABILITIES,
  UMI_UFMP_NETWORK_HEARTBEAT,
  UMI_UFMP_PARTICIPANT_LOOKUP,
  UMI_UFMP_PARTICIPANT_STATUS,
  UMI_UFMP_ROUTE_CAPABILITIES,
  UMI_UFMP_MESSAGE_RECEIVED,
  UMI_UFMP_MESSAGE_REJECTED,
  UMI_UFMP_MESSAGE_DELIVERY_STATUS,
  UMI_UFMP_FX_QUOTE_REQUEST,
  UMI_UFMP_FX_QUOTE_OFFER,
  UMI_UFMP_FX_QUOTE_ACCEPT,
  UMI_UFMP_FX_QUOTE_EXPIRE,
  UMI_UFMP_PAYMENT_INITIATE,
  UMI_UFMP_PAYMENT_ACCEPT,
  UMI_UFMP_PAYMENT_REJECT,
  UMI_UFMP_PAYMENT_STATUS,
  UMI_UFMP_PAYMENT_CANCEL_REQUEST,
  UMI_UFMP_PAYMENT_RETURN,
  UMI_UFMP_PAYMENT_INVESTIGATION,
  UMI_UFMP_CLEARING_OBLIGATION,
  UMI_UFMP_CLEARING_POSITION,
  UMI_UFMP_CLEARING_CONFIRM,
  UMI_UFMP_SETTLEMENT_INSTRUCT,
  UMI_UFMP_SETTLEMENT_STATUS,
  UMI_UFMP_SETTLEMENT_CONFIRM,
  UMI_UFMP_RECONCILIATION_STATEMENT,
  UMI_UFMP_RECONCILIATION_DISCREPANCY,
  UMI_UFMP_RISK_LIMIT_STATUS,
  UMI_UFMP_COMPLIANCE_CASE_STATUS
} UmiUfmpMessageType;
typedef enum UmiUfmpPayloadFormat {
  UMI_UFMP_PAYLOAD_ISO20022_XML = 1,
  UMI_UFMP_PAYLOAD_CONTROL_CBOR = 2
} UmiUfmpPayloadFormat;
/* A type name describes semantics only. It grants no institution permission. */
const char *UmiUfmpMessageTypeName(UmiUfmpMessageType type);
UmiStatus UmiUfmpMessageTypeParse(const char *name, size_t length, UmiUfmpMessageType *out);
int UmiUfmpMessageRequiresIdempotency(UmiUfmpMessageType type);
/* Opaque identifiers: 1..64 ASCII letters/digits/dot/underscore/colon/hyphen.
 * Passing capacity bounds the read; a missing terminator is rejected. This
 * grammar does not verify a bank, BIC, account or legal entity. */
int UmiUfmpIdentifierValid(const char *text, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
