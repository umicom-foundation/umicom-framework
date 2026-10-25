/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/types.c
 *
 * PURPOSE:
 *   Resolve explicit financial message semantics and bounded opaque identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/types.h"
#include <string.h>
typedef struct UmiUfmpTypeEntry {
  UmiUfmpMessageType type;
  const char *name;
  int needs_key;
} UmiUfmpTypeEntry;
static const UmiUfmpTypeEntry Types[] = {
    {UMI_UFMP_NETWORK_HELLO, "network.hello", 0},
    {UMI_UFMP_NETWORK_CAPABILITIES, "network.capabilities", 0},
    {UMI_UFMP_NETWORK_HEARTBEAT, "network.heartbeat", 0},
    {UMI_UFMP_PARTICIPANT_LOOKUP, "participant.lookup", 0},
    {UMI_UFMP_PARTICIPANT_STATUS, "participant.status", 0},
    {UMI_UFMP_ROUTE_CAPABILITIES, "route.capabilities", 0},
    {UMI_UFMP_MESSAGE_RECEIVED, "message.received", 0},
    {UMI_UFMP_MESSAGE_REJECTED, "message.rejected", 0},
    {UMI_UFMP_MESSAGE_DELIVERY_STATUS, "message.deliveryStatus", 0},
    {UMI_UFMP_FX_QUOTE_REQUEST, "fx.quoteRequest", 1},
    {UMI_UFMP_FX_QUOTE_OFFER, "fx.quoteOffer", 1},
    {UMI_UFMP_FX_QUOTE_ACCEPT, "fx.quoteAccept", 1},
    {UMI_UFMP_FX_QUOTE_EXPIRE, "fx.quoteExpire", 1},
    {UMI_UFMP_PAYMENT_INITIATE, "payment.initiate", 1},
    {UMI_UFMP_PAYMENT_ACCEPT, "payment.accept", 1},
    {UMI_UFMP_PAYMENT_REJECT, "payment.reject", 1},
    {UMI_UFMP_PAYMENT_STATUS, "payment.status", 0},
    {UMI_UFMP_PAYMENT_CANCEL_REQUEST, "payment.cancelRequest", 1},
    {UMI_UFMP_PAYMENT_RETURN, "payment.return", 1},
    {UMI_UFMP_PAYMENT_INVESTIGATION, "payment.investigation", 1},
    {UMI_UFMP_CLEARING_OBLIGATION, "clearing.obligation", 1},
    {UMI_UFMP_CLEARING_POSITION, "clearing.position", 1},
    {UMI_UFMP_CLEARING_CONFIRM, "clearing.confirm", 1},
    {UMI_UFMP_SETTLEMENT_INSTRUCT, "settlement.instruct", 1},
    {UMI_UFMP_SETTLEMENT_STATUS, "settlement.status", 0},
    {UMI_UFMP_SETTLEMENT_CONFIRM, "settlement.confirm", 1},
    {UMI_UFMP_RECONCILIATION_STATEMENT, "reconciliation.statement", 1},
    {UMI_UFMP_RECONCILIATION_DISCREPANCY, "reconciliation.discrepancy", 1},
    {UMI_UFMP_RISK_LIMIT_STATUS, "risk.limitStatus", 0},
    {UMI_UFMP_COMPLIANCE_CASE_STATUS, "compliance.caseStatus", 0}};
static const UmiUfmpTypeEntry *Find(UmiUfmpMessageType type) {
  for (size_t i = 0U; i < sizeof Types / sizeof Types[0]; ++i)
    if (Types[i].type == type)
      return &Types[i];
  return NULL;
}
const char *UmiUfmpMessageTypeName(UmiUfmpMessageType type) {
  const UmiUfmpTypeEntry *entry = Find(type);
  return entry == NULL ? NULL : entry->name;
}
UmiStatus UmiUfmpMessageTypeParse(const char *name, size_t length, UmiUfmpMessageType *out) {
  if (name == NULL || out == NULL || length == 0U || length > 64U)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0U; i < sizeof Types / sizeof Types[0]; ++i)
    if (strlen(Types[i].name) == length && memcmp(Types[i].name, name, length) == 0) {
      *out = Types[i].type;
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
int UmiUfmpMessageRequiresIdempotency(UmiUfmpMessageType type) {
  const UmiUfmpTypeEntry *entry = Find(type);
  /* Unknown actions never get a permissive default. Syntax validation also
   * rejects them before a digest or replay comparison can be performed. */
  return entry == NULL ? 1 : entry->needs_key;
}
int UmiUfmpIdentifierValid(const char *text, size_t capacity) {
  if (text == NULL || capacity < 2U)
    return 0;
  if (capacity > UMI_UFMP_ID_CAPACITY)
    capacity = UMI_UFMP_ID_CAPACITY;
  for (size_t i = 0U; i < capacity; ++i) {
    unsigned char c = (unsigned char)text[i];
    if (c == 0U)
      return i != 0U;
    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' ||
          c == '_' || c == ':' || c == '-'))
      return 0;
  }
  return 0;
}
