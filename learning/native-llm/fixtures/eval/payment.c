/* Umicom Foundation | MIT | Held-out fictional payment validation. */
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
