/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/internal.h
 *
 * PURPOSE:
 *   Define private ownership and transactional state shared by the financial operations domain modules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_INTERNAL_H
#define UMICOM_FINANCE_OPERATIONS_INTERNAL_H
#include "umicom/finance_operations/operations.h"

#define FINANCE_EVENT_PREFIX "umicom.finance-operations.event."
#define FINANCE_REVISION_KEY "umicom.finance-operations.revision"
#define FINANCE_RECORD_CAPACITY 4096U
#define FINANCE_INDEX_NONE SIZE_MAX

/* The Master Controller commits one candidate state. Accounting, matching,
 * clearing and repository Slave Controllers own their narrow operations.
 * Heap-backed copies keep failed multi-leg commands out of the visible state. */
typedef struct FinanceState {
    UmiFinanceOperationCounts counts;
    UmiFinanceOperationAccount accounts[UMI_FINANCE_OPERATIONS_ACCOUNTS];
    UmiFinanceOperationPeriod periods[UMI_FINANCE_OPERATIONS_PERIODS];
    UmiFinanceOperationJournal journals[UMI_FINANCE_OPERATIONS_JOURNALS];
    UmiFinanceOperationParticipant participants[UMI_FINANCE_OPERATIONS_PARTICIPANTS];
    UmiFinanceOperationInstrument instruments[UMI_FINANCE_OPERATIONS_INSTRUMENTS];
    UmiFinanceOperationHolding holdings[UMI_FINANCE_OPERATIONS_HOLDINGS];
    UmiFinanceOperationOrder orders[UMI_FINANCE_OPERATIONS_ORDERS];
    UmiFinanceOperationFill fills[UMI_FINANCE_OPERATIONS_FILLS];
    UmiFinanceOperationReconciliation reconciliations[UMI_FINANCE_OPERATIONS_RECONCILIATIONS];
    UmiFinanceOperationCommand commands[UMI_FINANCE_OPERATIONS_EVENTS];
} FinanceState;

struct UmiFinanceOperations {
    UmiDataServer *server;
    FinanceState *state;
    bool poisoned;
    bool writesBlocked; /* A failed reload leaves the old view readable, not writable. */
};

/* Shared validation is deliberately independent of GTK and storage. */
bool FinanceTextValid(const char *text, size_t capacity, bool required);
bool FinanceIdValid(const UmiFinancialId *id, bool required);
bool FinanceCurrencyValid(UmiCurrency currency);
bool FinanceDateInPeriod(UmiFinancialDate date, const UmiFinanceOperationPeriod *period);
UmiStatus FinanceCommandValidate(const UmiFinanceOperationCommand *command);
UmiStatus FinanceAdd(int64_t left, int64_t right, int64_t *out);
UmiStatus FinanceMultiplyPositive(int64_t left, int64_t right, int64_t *out);
UmiStatus FinanceCost(const UmiFinanceOperationInstrument *instrument,
    int64_t ticks, int64_t lots, int64_t *out);
UmiStatus FinanceSetId(UmiFinancialId *id, const char *text);
bool FinanceIdEqual(UmiFinancialId left, UmiFinancialId right);
size_t FinanceAccountIndex(const FinanceState *, UmiFinancialId);
size_t FinancePeriodIndex(const FinanceState *, UmiFinancialId);
size_t FinanceJournalIndex(const FinanceState *, UmiFinancialId);
size_t FinanceParticipantIndex(const FinanceState *, UmiFinancialId);
size_t FinanceInstrumentIndex(const FinanceState *, UmiFinancialId);
size_t FinanceOrderIndex(const FinanceState *, UmiFinancialId);
size_t FinanceFillIndex(const FinanceState *, UmiFinancialId);
size_t FinanceHoldingIndex(const FinanceState *, UmiFinancialId, UmiFinancialId);
UmiStatus FinanceAccountBalance(const FinanceState *, UmiFinancialId, int64_t *out);
UmiStatus FinanceCash(const FinanceState *, UmiFinancialId, UmiFinanceOperationAvailability *out);
UmiStatus FinanceLots(const FinanceState *, UmiFinancialId, UmiFinancialId, UmiFinanceOperationAvailability *out);
UmiStatus FinanceTrialBalance(const FinanceState *, UmiFinancialId, UmiCurrency, uint8_t, UmiFinanceTrialBalance *);
UmiStatus FinanceAllReservationsValid(const FinanceState *);
UmiStatus FinanceApplyAccounting(FinanceState *, const UmiFinanceOperationCommand *);
UmiStatus FinanceApplyExchange(FinanceState *, const UmiFinanceOperationCommand *);
UmiStatus FinanceApplySettlement(FinanceState *, const UmiFinanceOperationCommand *);
UmiStatus FinancePostJournal(FinanceState *, UmiFinanceOperationJournal *);
UmiStatus FinanceExecute(FinanceState *, const UmiFinanceOperationCommand *);
UmiStatus FinanceEncode(const UmiFinanceOperationCommand *, char *, size_t);
UmiStatus FinanceDecode(const char *, UmiFinanceOperationCommand *);
bool FinanceSameCommand(const UmiFinanceOperationCommand *, const UmiFinanceOperationCommand *);
UmiStatus FinanceRepositoryLoad(UmiFinanceOperations *, FinanceState **out);
UmiStatus FinanceRepositoryCommit(UmiFinanceOperations *, const UmiFinanceOperationCommand *);
#endif
