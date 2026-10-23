/* Umicom Foundation | MIT | Held-out fictional payment validation.
 * File: learning/native-llm/fixtures/eval/payment.c
 * PURPOSE: Provide a fictional held-out payment-validation sample for native language-model evaluation.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 */
#include <stdint.h>
#include <stdbool.h>

typedef struct UmiLearningPayment {
    uint64_t paymentId;
    uint64_t destinationId;
    int64_t amountMinorUnits;
} UmiLearningPayment;

bool UmiLearningPaymentValid(const UmiLearningPayment *payment)
{
    return payment != 0 && payment->paymentId != 0U &&
           payment->destinationId != 0U && payment->amountMinorUnits > 0;
}
