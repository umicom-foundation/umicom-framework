/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_operations/operations.h
 *
 * PURPOSE:
 *   Expose shared accounting, order matching, clearing and local settlement operations through a C23 service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_OPERATIONS_H
#define UMICOM_FINANCE_OPERATIONS_OPERATIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/finance/accounting/journal_entry.h"
#include "umicom/trading/core/matching_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCE_OPERATIONS_ACCOUNTS 64U
#define UMI_FINANCE_OPERATIONS_PERIODS 16U
#define UMI_FINANCE_OPERATIONS_INSTRUMENTS 32U
#define UMI_FINANCE_OPERATIONS_PARTICIPANTS 64U
#define UMI_FINANCE_OPERATIONS_HOLDINGS 128U
#define UMI_FINANCE_OPERATIONS_ORDERS 128U
#define UMI_FINANCE_OPERATIONS_FILLS 128U
#define UMI_FINANCE_OPERATIONS_JOURNALS 128U
#define UMI_FINANCE_OPERATIONS_RECONCILIATIONS 128U
#define UMI_FINANCE_OPERATIONS_EVENTS 512U
#define UMI_FINANCE_OPERATIONS_JOURNAL_LINES 8U

/** One accepted command becomes one durable event. This is a local simulation:
 * actor names support workflow separation, not authenticated authorisation.
 * Only limit/GTC orders, whole lots and cash-backed settlement are supported. */
typedef enum UmiFinanceOperationKind {
    UMI_FINANCE_CREATE_ACCOUNT = 1,
    UMI_FINANCE_OPEN_PERIOD,
    UMI_FINANCE_ENTER_JOURNAL,
    UMI_FINANCE_APPROVE_JOURNAL,
    UMI_FINANCE_POST_JOURNAL,
    UMI_FINANCE_REVERSE_JOURNAL,
    UMI_FINANCE_REGISTER_PARTICIPANT,
    UMI_FINANCE_LIST_INSTRUMENT,
    UMI_FINANCE_SET_MARKET_STATE,
    UMI_FINANCE_DEPOSIT_LOTS,
    UMI_FINANCE_PLACE_ORDER,
    UMI_FINANCE_CANCEL_ORDER,
    UMI_FINANCE_CLEAR_FILL,
    UMI_FINANCE_SETTLE_FILL,
    UMI_FINANCE_RECONCILE_ACCOUNT,
    UMI_FINANCE_PREPARE_CLOSE,
    UMI_FINANCE_CLOSE_PERIOD,
    UMI_FINANCE_REOPEN_REVIEW
} UmiFinanceOperationKind;

/** A command journal line borrows no memory. The service constructs the existing
 * canonical UmiAccountingJournalLine and checks exact totals before posting. */
typedef struct UmiFinanceOperationLine {
    UmiFinancialId accountId;
    int64_t debitMinor;
    int64_t creditMinor;
} UmiFinanceOperationLine;

/** A serialisable command with bounded values. Initialise before filling it.
 *
 * Common: requestId, actorId, id and expectedRevision.
 * CREATE_ACCOUNT: name, currency, scale, accountClass.
 * OPEN_PERIOD: date (first day), endDate (last day), non-overlapping ranges.
 * ENTER_JOURNAL: date, referenceId (period), lines and lineCount.
 * APPROVE/POST_JOURNAL: id of an existing journal.
 * REVERSE_JOURNAL: id of a NEW compensating journal, referenceId of the original,
 *   periodId of an OPEN correction period, date. A different actor must approve
 *   and post the compensating journal before the original gains reversedBy.
 * REGISTER_PARTICIPANT: accountId of a unique liability cash account; name.
 * LIST_INSTRUMENT: name, currency, scale, minorPerTick, unitsPerLot, maxOrderLots.
 * SET_MARKET_STATE: id of the instrument; enabled (true=open, false=halted).
 * DEPOSIT_LOTS: id of a participant, instrumentId, lots; simulation inventory only.
 * PLACE_ORDER: id of a NEW order, participantId, instrumentId, side, priceTicks,
 *   lots and date. A fill executes at the older resting order's limit price.
 * CANCEL_ORDER: id of an existing order, participantId (ownership check).
 * CLEAR_FILL: id of a fill. SETTLE_FILL: id, periodId and date; a different actor
 *   from the clearing actor is required. Cash and asset legs commit together.
 * RECONCILE_ACCOUNT: id of new evidence, accountId, amountMinor (normal-side
 *   external balance). Evidence never changes ledger balances.
 * PREPARE_CLOSE/CLOSE_PERIOD/REOPEN_REVIEW: id of a period. A different actor
 *   closes a prepared review; a final closed period cannot be reopened here.
 * Unused fields should remain zero. Reusing a request with any changed payload
 * is a conflict; expectedRevision alone is excluded from duplicate comparison. */
typedef struct UmiFinanceOperationCommand {
    UmiFinanceOperationKind kind;
    uint64_t expectedRevision;
    UmiFinancialId requestId;
    UmiFinancialId actorId;
    UmiFinancialId id;
    UmiFinancialId referenceId;
    UmiFinancialId periodId;
    UmiFinancialId accountId;
    UmiFinancialId participantId;
    UmiFinancialId instrumentId;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiCurrency currency;
    uint8_t scale;
    UmiAccountingAccountClass accountClass;
    UmiSide side;
    bool enabled;
    UmiFinancialDate date;
    UmiFinancialDate endDate;
    int64_t amountMinor;
    UmiTradingPriceTicks priceTicks;
    UmiTradingQuantityLots lots;
    int64_t minorPerTick;
    int64_t unitsPerLot;
    int64_t maxOrderLots;
    size_t lineCount;
    UmiFinanceOperationLine lines[UMI_FINANCE_OPERATIONS_JOURNAL_LINES];
} UmiFinanceOperationCommand;

/** A copied receipt identifies the original event even after a duplicate retry. */
typedef struct UmiFinanceOperationReceipt {
    uint64_t revision;
    UmiFinancialId requestId;
    UmiFinancialId entityId;
    bool duplicate;
} UmiFinanceOperationReceipt;

/** These records belong to Framework, not to either frontend. Journal entries
 * reuse the canonical accounting model; account totals are replayed projections
 * of posted entries, never a second independently editable ledger. */
typedef struct UmiFinanceOperationAccount {
    UmiFinancialId id;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiCurrency currency;
    uint8_t scale;
    UmiAccountingAccountClass accountClass;
    int64_t debitMinor;
    int64_t creditMinor;
    uint64_t lastPostingRevision;
} UmiFinanceOperationAccount;

typedef struct UmiFinanceOperationPeriod {
    UmiFinancialId id;
    UmiFinancialDate startDate;
    UmiFinancialDate endDate;
    UmiAccountingPeriodStatus status;
    UmiFinancialId preparedBy;
    UmiFinancialId closedBy;
} UmiFinanceOperationPeriod;

typedef struct UmiFinanceOperationJournal {
    UmiAccountingJournalEntry entry;
    UmiFinancialId periodId;
    UmiFinancialId makerId;
    UmiFinancialId checkerId;
    UmiFinancialId reverses;
    UmiFinancialId reversedBy;
    UmiFinancialId fillId;
    UmiCurrency currency;
    uint8_t scale;
    uint64_t postingRevision;
    bool settlement;
} UmiFinanceOperationJournal;

typedef struct UmiFinanceOperationParticipant {
    UmiFinancialId id;
    UmiFinancialId cashAccountId;
    char name[UMI_FINANCE_NAME_CAPACITY];
} UmiFinanceOperationParticipant;

typedef struct UmiFinanceOperationInstrument {
    UmiFinancialId id;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiCurrency currency;
    uint8_t scale;
    int64_t minorPerTick;
    int64_t unitsPerLot;
    int64_t maxOrderLots;
    UmiMarketState marketState;
} UmiFinanceOperationInstrument;

typedef struct UmiFinanceOperationHolding {
    UmiFinancialId participantId;
    UmiFinancialId instrumentId;
    UmiTradingQuantityLots lots;
} UmiFinanceOperationHolding;

typedef struct UmiFinanceOperationOrder {
    UmiFinancialId id;
    UmiFinancialId participantId;
    UmiFinancialId instrumentId;
    UmiSide side;
    UmiTradingPriceTicks limitTicks;
    UmiTradingQuantityLots originalLots;
    UmiTradingQuantityLots remainingLots;
    UmiTradingQuantityLots filledLots;
    UmiOrderStatus status;
    UmiFinancialDate date;
    uint64_t sequence;
} UmiFinanceOperationOrder;

typedef struct UmiFinanceOperationFill {
    UmiFinancialId id;
    UmiFinancialId buyOrderId;
    UmiFinancialId sellOrderId;
    UmiFinancialId buyerId;
    UmiFinancialId sellerId;
    UmiFinancialId instrumentId;
    UmiFinancialId clearingActor;
    UmiFinancialId settlementJournalId;
    UmiTradingPriceTicks executionTicks;
    UmiTradingQuantityLots lots;
    UmiMoney cash;
    UmiFinancialDate tradeDate;
    UmiFinancialDate settlementDate;
    UmiSettlementState state;
} UmiFinanceOperationFill;

typedef struct UmiFinanceOperationReconciliation {
    UmiFinancialId id;
    UmiFinancialId accountId;
    UmiFinancialId actorId;
    int64_t externalMinor;
    int64_t ledgerMinor;
    uint64_t postingRevision;
    uint64_t revision;
    bool matched;
} UmiFinanceOperationReconciliation;

typedef struct UmiFinanceOperationAudit {
    UmiFinancialId requestId;
    UmiFinancialId actorId;
    UmiFinancialId entityId;
    UmiFinanceOperationKind kind;
    uint64_t revision;
} UmiFinanceOperationAudit;

typedef struct UmiFinanceOperationCounts {
    size_t accounts, periods, journals, participants, instruments;
    size_t holdings, orders, fills, reconciliations, events;
    uint64_t revision;
    bool durable;
} UmiFinanceOperationCounts;

/** Cash is read from the participant's posted liability account. Reservations
 * include open buy limits and matched-but-unsettled executions. They are not
 * postings. Lots follow the corresponding inventory reservation rule. */
typedef struct UmiFinanceOperationAvailability {
    int64_t booked;
    int64_t reserved;
    int64_t available;
} UmiFinanceOperationAvailability;

typedef struct UmiFinanceTrialBalanceLine {
    UmiFinancialId accountId;
    UmiAccountingAccountClass accountClass;
    int64_t openingDebitNet;
    int64_t periodDebitMinor;
    int64_t periodCreditMinor;
    int64_t closingDebitNet;
} UmiFinanceTrialBalanceLine;

typedef struct UmiFinanceTrialBalance {
    UmiFinancialId periodId;
    UmiCurrency currency;
    uint8_t scale;
    size_t lineCount;
    UmiFinanceTrialBalanceLine lines[UMI_FINANCE_OPERATIONS_ACCOUNTS];
    int64_t periodDebitMinor, periodCreditMinor;
    int64_t closingDebitMinor, closingCreditMinor;
    bool balanced;
} UmiFinanceTrialBalance;

typedef struct UmiFinanceOperations UmiFinanceOperations;

/** Initialise caller-owned command storage without creating records or files. */
void UmiFinanceOperationCommandInit(UmiFinanceOperationCommand *command);
/** The server is borrowed, exclusively used by this service, and must outlive
 * it. Each window uses its own server handle. The service is single-thread-owned.
 * SQLite handles may share one database; optimistic revision checks reject stale
 * writers. No SQLite failure is converted into memory-only success. */
UmiStatus UmiFinanceOperationsCreate(UmiDataServer *server, UmiFinanceOperations **out);
/** Release the service; the caller still owns the Data Server. */
void UmiFinanceOperationsDestroy(UmiFinanceOperations *operations);
/** Replay persisted events into a new state before replacing the current view.
 * A failed reload leaves the last known view readable but blocks writes until
 * a complete reload succeeds. Storage corruption is never treated as empty. */
UmiStatus UmiFinanceOperationsReload(UmiFinanceOperations *operations);
/** Validate and commit one all-or-nothing event. Rejected requests leave the
 * financial state and revision unchanged. A poisoned storage handle exposes no
 * further queries or writes and must be closed and reopened by its owner. */
UmiStatus UmiFinanceOperationsApply(UmiFinanceOperations *operations,
    const UmiFinanceOperationCommand *command, UmiFinanceOperationReceipt *out);
/** Copy record counts and the local view's revision. */
UmiStatus UmiFinanceOperationsCounts(const UmiFinanceOperations *operations,
    UmiFinanceOperationCounts *out);
/** Indexed queries return copied records; NOT_FOUND means index is out of range. */
UmiStatus UmiFinanceOperationsAccountAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationAccount *);
UmiStatus UmiFinanceOperationsPeriodAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationPeriod *);
UmiStatus UmiFinanceOperationsJournalAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationJournal *);
UmiStatus UmiFinanceOperationsParticipantAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationParticipant *);
UmiStatus UmiFinanceOperationsInstrumentAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationInstrument *);
UmiStatus UmiFinanceOperationsHoldingAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationHolding *);
UmiStatus UmiFinanceOperationsOrderAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationOrder *);
UmiStatus UmiFinanceOperationsFillAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationFill *);
UmiStatus UmiFinanceOperationsReconciliationAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationReconciliation *);
UmiStatus UmiFinanceOperationsAuditAt(const UmiFinanceOperations *, size_t, UmiFinanceOperationAudit *);
/** Copy normal-side balance: assets/expenses are debit-normal; other classes
 * are credit-normal. No floating point or cross-currency conversion is used. */
UmiStatus UmiFinanceOperationsAccountBalance(const UmiFinanceOperations *, const char *accountId, int64_t *out);
UmiStatus UmiFinanceOperationsCash(const UmiFinanceOperations *, const char *participantId, UmiFinanceOperationAvailability *out);
UmiStatus UmiFinanceOperationsLots(const UmiFinanceOperations *, const char *participantId, const char *instrumentId, UmiFinanceOperationAvailability *out);
/** One currency and scale per report. Opening and closing balances are derived
 * from dated posted journals, including original and compensating entries. */
UmiStatus UmiFinanceOperationsTrialBalance(const UmiFinanceOperations *, const char *periodId,
    UmiCurrency currency, uint8_t scale, UmiFinanceTrialBalance *out);
/** Format a money amount exactly, including INT64_MIN, into caller storage. */
UmiStatus UmiFinanceOperationsFormatAmount(int64_t minor, uint8_t scale, char *buffer, size_t capacity);
/** Readable names shared by command forms, examples and diagnostics. */
const char *UmiFinanceOperationKindText(UmiFinanceOperationKind kind);

#ifdef __cplusplus
}
#endif
#endif
