/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/operations.h
 *
 * PURPOSE:
 *   Expose persistent, controlled banking workflows over canonical finance types.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_OPERATIONS_OPERATIONS_H
#define UMICOM_BANK_OPERATIONS_OPERATIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#include "umicom/finance/core/types.h"
#include "umicom/finance/accounting/journal_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BANK_RECORD_CAPACITY 64U
#define UMI_BANK_EVENT_CAPACITY 256U
#define UMI_BANK_STATEMENT_CAPACITY UMI_BANK_EVENT_CAPACITY

/** Opaque, serially accessed banking service. It owns its Data Server and all
 * state; no pointer returned by the API grants mutable access to that state.
 * Use one serial command queue per handle. Separate SQLite handles detect stale
 * revisions inside a Data Server transaction and return UMI_STATUS_BUSY.
 *
 * This release is an explicitly local simulation profile. It neither connects
 * to payment/card networks nor authenticates people. Trusted application or
 * identity middleware supplies actor capabilities. Never expose that argument
 * directly to an untrusted network client. */
typedef struct UmiBankOperations UmiBankOperations;

/** Capabilities are checked before command execution and before idempotent
 * receipts are disclosed. Maker/checker identity separation is also enforced. */
typedef enum UmiBankCapability {
    UMI_BANK_CAP_CUSTOMERS = 1U,
    UMI_BANK_CAP_PAYMENTS = 2U,
    UMI_BANK_CAP_APPROVE = 4U,
    UMI_BANK_CAP_OPERATE = 8U,
    UMI_BANK_CAP_TEST_FUNDING = 16U,
    UMI_BANK_CAP_ALL = 31U
} UmiBankCapability;

typedef struct UmiBankActor {
    UmiFinancialId id;
    uint32_t capabilities;
} UmiBankActor;

/** Entity lifecycle. An account cannot close with a nonzero balance or hold.
 * Suspended customers and blocked accounts cannot initiate new debits. */
typedef enum UmiBankRecordState {
    UMI_BANK_RECORD_ACTIVE = 1,
    UMI_BANK_RECORD_BLOCKED = 2,
    UMI_BANK_RECORD_CLOSED = 3
} UmiBankRecordState;

typedef enum UmiBankTransferState {
    UMI_BANK_TRANSFER_PENDING = 1,
    UMI_BANK_TRANSFER_APPROVED = 2,
    UMI_BANK_TRANSFER_REJECTED = 3,
    UMI_BANK_TRANSFER_CANCELLED = 4,
    UMI_BANK_TRANSFER_EXECUTED = 5,
    UMI_BANK_TRANSFER_REVERSED = 6
} UmiBankTransferState;

typedef enum UmiBankHoldState {
    UMI_BANK_HOLD_ACTIVE = 1,
    UMI_BANK_HOLD_RELEASED = 2,
    UMI_BANK_HOLD_CAPTURED = 3,
    UMI_BANK_HOLD_REFUNDED = 4
} UmiBankHoldState;

/** Commands are shared Framework contracts, not application-specific handlers.
 * id identifies the entity; ownerId links a customer, beneficiary, card or hold
 * according to the action. sourceAccountId/destinationAccountId are used by
 * account, beneficiary, funding, transfer and hold operations. See
 * docs/BANK_OPERATIONS.html for the field map. All input strings must terminate within their arrays.
 * An amount is integer minor units, never binary floating point. */
typedef enum UmiBankAction {
    UMI_BANK_CUSTOMER_CREATE = 1,
    UMI_BANK_CUSTOMER_SET_STATE,
    UMI_BANK_ACCOUNT_OPEN,
    UMI_BANK_ACCOUNT_SET_STATE,
    UMI_BANK_BENEFICIARY_CREATE,
    UMI_BANK_BENEFICIARY_SET_STATE,
    UMI_BANK_TEST_CREDIT,
    UMI_BANK_TRANSFER_SUBMIT,
    UMI_BANK_TRANSFER_APPROVE,
    UMI_BANK_TRANSFER_REJECT,
    UMI_BANK_TRANSFER_CANCEL,
    UMI_BANK_TRANSFER_EXECUTE,
    UMI_BANK_TRANSFER_REVERSE,
    UMI_BANK_HOLD_PLACE,
    UMI_BANK_HOLD_RELEASE,
    UMI_BANK_CARD_ISSUE,
    UMI_BANK_CARD_SET_STATE,
    UMI_BANK_CARD_AUTHORISE,
    UMI_BANK_CARD_CAPTURE,
    UMI_BANK_CARD_VOID,
    UMI_BANK_CARD_REFUND,
    UMI_BANK_RECONCILE
} UmiBankAction;

/** Form metadata identifies the only payload fields accepted by an action.
 * This keeps GTK and other frontends aligned with the command boundary. */
typedef enum UmiBankCommandField {
    UMI_BANK_FIELD_NAME = 1U,
    UMI_BANK_FIELD_OWNER = 2U,
    UMI_BANK_FIELD_SOURCE = 4U,
    UMI_BANK_FIELD_DESTINATION = 8U,
    UMI_BANK_FIELD_MONEY = 16U,
    UMI_BANK_FIELD_STATE = 32U
} UmiBankCommandField;

typedef struct UmiBankCommand {
    UmiBankAction action;
    UmiFinancialId requestId;
    UmiFinancialId id;
    UmiFinancialId ownerId;
    UmiFinancialId sourceAccountId;
    UmiFinancialId destinationAccountId;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiMoney amount;
    UmiFinancialDate businessDate;
    int64_t timestampMillis;
    uint64_t expectedRevision;
    UmiBankRecordState state;
} UmiBankCommand;

typedef struct UmiBankReceipt {
    uint64_t revision;
    bool idempotent;
    UmiFinancialId requestId;
} UmiBankReceipt;

typedef struct UmiBankCustomer {
    UmiFinancialParty party;
    UmiBankRecordState state;
} UmiBankCustomer;

typedef struct UmiBankAccount {
    UmiFinancialAccount account;
    char name[UMI_FINANCE_NAME_CAPACITY];
    uint8_t scale;
    UmiBankRecordState state;
} UmiBankAccount;

typedef struct UmiBankBalance {
    UmiMoney booked;
    UmiMoney reserved;
    UmiMoney available;
    uint64_t revision;
} UmiBankBalance;

typedef struct UmiBankBeneficiary {
    UmiFinancialId id;
    UmiFinancialId customerId;
    UmiFinancialId accountId;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiBankRecordState state;
} UmiBankBeneficiary;

typedef struct UmiBankTransfer {
    UmiFinancialId id;
    UmiFinancialId beneficiaryId;
    UmiFinancialId sourceAccountId;
    UmiFinancialId destinationAccountId;
    UmiFinancialId makerId;
    UmiFinancialId checkerId;
    UmiMoney amount;
    UmiBankTransferState state;
    uint64_t submittedRevision;
    uint64_t postedRevision;
} UmiBankTransfer;

typedef struct UmiBankCard {
    UmiFinancialId id;
    UmiFinancialId accountId;
    char name[UMI_FINANCE_NAME_CAPACITY];
    UmiMoney perTransactionLimit;
    UmiBankRecordState state;
} UmiBankCard;

typedef struct UmiBankHold {
    UmiFinancialId id;
    UmiFinancialId accountId;
    UmiFinancialId cardId; /* Empty for a manual hold. */
    UmiMoney amount;
    int64_t capturedMinor;
    UmiBankHoldState state;
    uint64_t postedRevision;
} UmiBankHold;

/** A currency and scale wrap the existing canonical accounting journal because
 * its lines intentionally contain integer amounts but not a currency field.
 * Customer deposit balances are credit-normal; original journals are retained
 * when a later compensating journal reverses their financial effect. */
typedef struct UmiBankJournal {
    UmiAccountingJournalEntry entry;
    UmiCurrency currency;
    uint8_t scale;
    uint64_t revision;
    UmiFinancialId referenceId;
    bool reversal;
} UmiBankJournal;

typedef struct UmiBankAuditEvent {
    uint64_t revision;
    UmiBankActor actor;
    UmiBankCommand command;
} UmiBankAuditEvent;

typedef struct UmiBankReconciliation {
    UmiFinancialId id;
    UmiFinancialId accountId;
    UmiMoney externalBalance;
    UmiMoney bookedBalance;
    uint64_t revision;
    bool matched;
} UmiBankReconciliation;

typedef struct UmiBankCounts {
    size_t customers;
    size_t accounts;
    size_t beneficiaries;
    size_t transfers;
    size_t cards;
    size_t holds;
    size_t journals;
    size_t reconciliations;
    size_t events;
    uint64_t revision;
    bool durable;
} UmiBankCounts;

typedef struct UmiBankStatementLine {
    uint64_t revision;
    UmiFinancialId journalId;
    UmiFinancialId referenceId;
    UmiFinancialDate businessDate;
    int64_t debitMinor;
    int64_t creditMinor;
    int64_t balanceMinor;
} UmiBankStatementLine;

typedef struct UmiBankStatement {
    UmiFinancialId accountId;
    UmiMoney opening;
    UmiMoney closing;
    uint64_t firstRevision;
    uint64_t lastRevision;
    size_t count;
    UmiBankStatementLine lines[UMI_BANK_STATEMENT_CAPACITY];
} UmiBankStatement;

/** Return allowed payload fields; identity, request and business time are
 * always required. Unused payload values are rejected rather than ignored. */
uint32_t UmiBankActionFields(UmiBankAction action);
/** Initialise a command without guessing a business date, actor or amount. */
void UmiBankCommandInit(UmiBankCommand *command, UmiBankAction action);
/** Stable descriptions are suitable for logs and non-localised test output. */
const char *UmiBankActionName(UmiBankAction action);
/** These factories create only the simulation profile. SQLite requires an
 * absolute local path; unavailable SQLite is reported, never replaced silently
 * by memory storage. Existing event streams are replayed and validated first. */
UmiStatus UmiBankOperationsOpenMemory(UmiBankOperations **outOperations);
UmiStatus UmiBankOperationsOpenSqlite(const char *absolutePath,
                                    UmiBankOperations **outOperations);
/** Destroy only after all calls on the serial command queue have completed. */
void UmiBankOperationsDestroy(UmiBankOperations *operations);
/** Reload is failure-atomic: malformed persistence never replaces valid state. */
UmiStatus UmiBankOperationsReload(UmiBankOperations *operations);
/** A successful non-idempotent command commits its event and revision through
 * one Data Server transaction before replacing in-memory state. A repeated
 * request must have the same actor identity and canonical payload (excluding
 * expectedRevision); conflicting reuse returns ALREADY_EXISTS. A stale local
 * or database revision returns BUSY: reload, review and resubmit deliberately.
 * On failure outReceipt is zero and no balance/hold/workflow change is applied. */
UmiStatus UmiBankOperationsExecute(UmiBankOperations *operations,
    const UmiBankActor *actor, const UmiBankCommand *command,
    UmiBankReceipt *outReceipt);
/** Query outputs are copies. None of these calls changes authoritative state. */
UmiStatus UmiBankOperationsCounts(const UmiBankOperations *operations,
                                UmiBankCounts *outCounts);
UmiStatus UmiBankOperationsBalance(const UmiBankOperations *operations,
    const char *accountId, UmiBankBalance *outBalance);
UmiStatus UmiBankOperationsStatement(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    UmiBankStatement *outStatement);
UmiStatus UmiBankOperationsCustomerAt(const UmiBankOperations *operations,
    size_t index, UmiBankCustomer *outCustomer);
UmiStatus UmiBankOperationsAccountAt(const UmiBankOperations *operations,
    size_t index, UmiBankAccount *outAccount);
UmiStatus UmiBankOperationsBeneficiaryAt(const UmiBankOperations *operations,
    size_t index, UmiBankBeneficiary *outBeneficiary);
UmiStatus UmiBankOperationsTransferAt(const UmiBankOperations *operations,
    size_t index, UmiBankTransfer *outTransfer);
UmiStatus UmiBankOperationsCardAt(const UmiBankOperations *operations,
    size_t index, UmiBankCard *outCard);
UmiStatus UmiBankOperationsHoldAt(const UmiBankOperations *operations,
    size_t index, UmiBankHold *outHold);
UmiStatus UmiBankOperationsJournalAt(const UmiBankOperations *operations,
    size_t index, UmiBankJournal *outJournal);
UmiStatus UmiBankOperationsAuditAt(const UmiBankOperations *operations,
    size_t index, UmiBankAuditEvent *outEvent);
UmiStatus UmiBankOperationsReconciliationAt(const UmiBankOperations *operations,
    size_t index, UmiBankReconciliation *outReconciliation);

#ifdef __cplusplus
}
#endif
#endif
