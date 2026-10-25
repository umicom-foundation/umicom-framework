/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_catalogue.c
 *
 * PURPOSE:
 *   Qualify known message names, bounded parsing and identifier limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  static const char *const names[] = {"roundtrip", "unknown", "bounded-name", "identifier",
                                      "requirements"};
  int c = Case(argc, argv, names, sizeof names / sizeof names[0]);
  if (c == 0) {
    for (int n = (int)UMI_UFMP_NETWORK_HELLO; n <= (int)UMI_UFMP_COMPLIANCE_CASE_STATUS; ++n) {
      UmiUfmpMessageType type = (UmiUfmpMessageType)n, parsed = (UmiUfmpMessageType)0;
      const char *name = UmiUfmpMessageTypeName(type);
      CHECK(name != NULL);
      OK(UmiUfmpMessageTypeParse(name, strlen(name), &parsed));
      CHECK(type == parsed);
    }
  } else if (c == 1) {
    UmiUfmpMessageType untouched = UMI_UFMP_NETWORK_HELLO;
    CHECK(UmiUfmpMessageTypeParse("payment.settled", 15U, &untouched) == UMI_STATUS_NOT_FOUND);
    CHECK(untouched == UMI_UFMP_NETWORK_HELLO);
    CHECK(UmiUfmpMessageTypeName((UmiUfmpMessageType)0) == NULL);
    CHECK(UmiUfmpMessageRequiresIdempotency((UmiUfmpMessageType)999));
  } else if (c == 2) {
    char bytes[64];
    memset(bytes, 'x', sizeof bytes);
    memcpy(bytes, "network.hello", 13U);
    UmiUfmpMessageType value;
    OK(UmiUfmpMessageTypeParse(bytes, 13U, &value));
    CHECK(value == UMI_UFMP_NETWORK_HELLO);
    CHECK(UmiUfmpMessageTypeParse(bytes, sizeof bytes, &value) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiUfmpMessageTypeParse(bytes, sizeof bytes + 1U, &value) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (c == 3) {
    char id[UMI_UFMP_ID_CAPACITY];
    memset(id, 'x', sizeof id);
    CHECK(!UmiUfmpIdentifierValid(id, sizeof id));
    id[sizeof id - 1U] = '\0';
    CHECK(UmiUfmpIdentifierValid(id, sizeof id));
    CHECK(!UmiUfmpIdentifierValid("", 1U));
    CHECK(!UmiUfmpIdentifierValid("bank/other", 11U));
    CHECK(!UmiUfmpIdentifierValid("Bank\nA", 7U));
    CHECK(!UmiUfmpIdentifierValid(NULL, 0U));
  } else {
    CHECK(UmiUfmpMessageRequiresIdempotency(UMI_UFMP_PAYMENT_INITIATE));
    CHECK(UmiUfmpMessageRequiresIdempotency(UMI_UFMP_PAYMENT_RETURN));
    CHECK(UmiUfmpMessageRequiresIdempotency(UMI_UFMP_FX_QUOTE_ACCEPT));
    CHECK(!UmiUfmpMessageRequiresIdempotency(UMI_UFMP_MESSAGE_RECEIVED));
    CHECK(!UmiUfmpMessageRequiresIdempotency(UMI_UFMP_NETWORK_HEARTBEAT));
  }
  return 0;
}
