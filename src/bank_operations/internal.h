/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/internal.h
 *
 * PURPOSE:
 *   Define private banking state, ownership and bounded module responsibilities.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_OPERATIONS_INTERNAL_H
#define UMICOM_BANK_OPERATIONS_INTERNAL_H
#include "umicom/bank_operations/operations.h"
#include "umicom/data/data_server.h"

#define BANK_RECORD_TEXT_CAPACITY 4096U
#define BANK_EVENT_KEY_PREFIX "bank.operations.event."
#define BANK_REVISION_KEY "bank.operations.revision"

typedef struct BankState {
    UmiBankCounts counts;
    UmiBankCustomer customers[UMI_BANK_RECORD_CAPACITY];
    UmiBankAccount accounts[UMI_BANK_RECORD_CAPACITY];
    UmiBankBeneficiary beneficiaries[UMI_BANK_RECORD_CAPACITY];
    UmiBankTransfer transfers[UMI_BANK_RECORD_CAPACITY];
    UmiBankCard cards[UMI_BANK_RECORD_CAPACITY];
    UmiBankHold holds[UMI_BANK_RECORD_CAPACITY];
    UmiBankJournal journals[UMI_BANK_EVENT_CAPACITY];
    UmiBankReconciliation reconciliations[UMI_BANK_RECORD_CAPACITY];
    UmiBankAuditEvent events[UMI_BANK_EVENT_CAPACITY];
} BankState;

struct UmiBankOperations {
    UmiDataServer *server; /* Owned; never exposed to application callers. */
    BankState *state;      /* Heap storage avoids a multi-megabyte C stack. */
    bool poisoned;        /* A failed rollback requires close/reopen. */
};

/** Codec owns a portable representation, never structure padding or pointers. */
UmiStatus BankEncode(const UmiBankAuditEvent *event, char *out, size_t capacity);
UmiStatus BankDecode(const char *text, UmiBankAuditEvent *out);
bool BankSameRequest(const UmiBankAuditEvent *event, const UmiBankActor *actor,
                     const UmiBankCommand *command);
UmiStatus BankCommandValid(const UmiBankActor *actor, const UmiBankCommand *command);
UmiStatus BankApply(BankState *state, const UmiBankActor *actor,
                   const UmiBankCommand *command);
UmiStatus BankProjectBalance(const BankState *state, const char *accountId,
                            UmiBankBalance *out);
UmiStatus BankPost(BankState *state, const UmiBankCommand *command,
    const char *debitAccount, const char *creditAccount, UmiMoney amount,
    bool reversal);
UmiStatus BankRepositoryLoad(UmiBankOperations *operations, BankState **outState);
UmiStatus BankRepositoryCommit(UmiBankOperations *operations,
    const UmiBankAuditEvent *event, uint64_t expectedRevision);
/** Internal finders return -1 for absence; public outputs remain copies. */
int BankFindCustomer(const BankState *state, const char *id);
int BankFindAccount(const BankState *state, const char *id);
int BankFindBeneficiary(const BankState *state, const char *id);
int BankFindTransfer(const BankState *state, const char *id);
int BankFindCard(const BankState *state, const char *id);
int BankFindHold(const BankState *state, const char *id);
bool BankIdValid(const UmiFinancialId *id, bool required);
bool BankMoneyMatches(UmiMoney amount, const UmiBankAccount *account);
UmiStatus BankAccountActive(const BankState *state, int index);
UmiStatus BankRequireFunds(const BankState *state, int accountIndex,
                          int64_t amountMinor);
UmiStatus BankApplyCustomerAccount(BankState *, const UmiBankActor *, const UmiBankCommand *);
UmiStatus BankApplyTransfer(BankState *, const UmiBankActor *, const UmiBankCommand *);
UmiStatus BankApplyCardHold(BankState *, const UmiBankActor *, const UmiBankCommand *);
#endif
