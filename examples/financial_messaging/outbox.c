/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/financial_messaging/outbox.c
 *
 * PURPOSE:
 *   Run the shared synthetic outgoing recovery example from an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/outbox.h"
#include <stdio.h>
int main(void) {
  char report[768]; size_t required = 0U;
  UmiStatus status = UmiUfmpOutboxExampleFormat(report, sizeof report, &required);
  if (status != UMI_STATUS_OK) {
    fprintf(stderr, "Outgoing recovery example: %d\n", (int)status);
    return 1;
  }
  return fputs(report, stdout) == EOF ? 1 : 0;
}
