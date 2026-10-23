/* Umicom Foundation | MIT | Illustrative, fictional accounts in C23.
 * File: learning/native-llm/fixtures/train/account.c
 * PURPOSE: Provide a fictional account-credit sample with bounded integer arithmetic for native training.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 */
#include <stdint.h>
#include <stdbool.h>

typedef struct UmiLearningAccount {
    uint64_t accountId;
    int64_t minorUnits;
} UmiLearningAccount;

bool UmiLearningAccountCredit(UmiLearningAccount *account, int64_t amount)
{
    if (account == 0 || amount < 0 || account->minorUnits > INT64_MAX - amount)
        return false;
    account->minorUnits += amount;
    return true;
}
