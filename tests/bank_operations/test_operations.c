/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_operations/test_operations.c
 *
 * PURPOSE:
 *   Test banking transitions, financial invariants, persistence, replay and storage failures.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "../../src/bank_operations/internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These checks execute in Release too; assert() would erase validation under
 * NDEBUG. Persistence tests use the actual Framework Data Server, not a stub. */
#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); exit(1); \
} } while (0)
#define OK(expression) CHECK((expression) == UMI_STATUS_OK)

typedef struct Fixture { UmiBankOperations *operations; uint64_t serial; } Fixture;
static UmiBankActor maker = {{{0}}, UMI_BANK_CAP_ALL};
static UmiBankActor checker = {{{0}}, UMI_BANK_CAP_ALL};
static UmiBankActor other = {{{0}}, UMI_BANK_CAP_PAYMENTS};

static void Id(UmiFinancialId *id, const char *text) { OK(umi_financial_id_assign(id, text)); }
static UmiMoney Money(int64_t amount, const char *currency, uint8_t scale)
{
    UmiMoney result = {0}; result.minor_units = amount; result.scale = scale;
    OK(umi_accounting_currency_from_code(currency, &result.currency)); return result;
}
static UmiBankCommand Command(Fixture *fixture, UmiBankAction action, const char *id)
{
    UmiBankCommand command;
    UmiBankCounts counts;
    UmiBankCommandInit(&command, action);
    OK(UmiBankOperationsCounts(fixture->operations, &counts));
    (void)snprintf(command.requestId.value, sizeof command.requestId.value,
        "request-%" PRIu64, ++fixture->serial);
    Id(&command.id, id);
    command.businessDate = (UmiFinancialDate){2026, 9U, 25U};
    command.timestampMillis = (int64_t)fixture->serial;
    command.expectedRevision = counts.revision;
    return command;
}
static UmiStatus Send(Fixture *fixture, const UmiBankActor *actor, const UmiBankCommand *command)
{
    UmiBankReceipt receipt;
    UmiStatus status;
    memset(&receipt, 0x7f, sizeof receipt);
    status = UmiBankOperationsExecute(fixture->operations, actor, command, &receipt);
    if (status != UMI_STATUS_OK) CHECK(receipt.revision == 0U && receipt.requestId.value[0] == '\0');
    return status;
}
static void Customer(Fixture *fixture, const char *id)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_CUSTOMER_CREATE, id);
    (void)snprintf(c.name, sizeof c.name, "Umicom test customer %s", id); OK(Send(fixture, &maker, &c));
}
static void Account(Fixture *fixture, const char *id, const char *owner, const char *currency, uint8_t scale)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_ACCOUNT_OPEN, id);
    Id(&c.ownerId, owner); (void)snprintf(c.name, sizeof c.name, "Account %s", id);
    c.amount = Money(0, currency, scale); OK(Send(fixture, &maker, &c));
}
static void Beneficiary(Fixture *fixture, const char *id, const char *owner, const char *destination)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_BENEFICIARY_CREATE, id);
    Id(&c.ownerId, owner); Id(&c.destinationAccountId, destination);
    (void)snprintf(c.name, sizeof c.name, "Beneficiary %s", id); OK(Send(fixture, &maker, &c));
}
static UmiBankCommand CreditCommand(Fixture *fixture, int64_t amount)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_TEST_CREDIT, "funding");
    Id(&c.sourceAccountId, "a1"); c.amount = Money(amount, "GBP", 2U); return c;
}
static void Credit(Fixture *fixture, int64_t amount)
{
    UmiBankCommand c = CreditCommand(fixture, amount); OK(Send(fixture, &maker, &c));
}
static UmiBankCommand SubmitCommand(Fixture *fixture, const char *id, int64_t amount)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_TRANSFER_SUBMIT, id);
    Id(&c.ownerId, "b1"); Id(&c.sourceAccountId, "a1"); c.amount = Money(amount, "GBP", 2U); return c;
}
static void TransferAction(Fixture *fixture, UmiBankAction action, const char *id, const UmiBankActor *actor)
{
    UmiBankCommand c = Command(fixture, action, id); OK(Send(fixture, actor, &c));
}
static void Submit(Fixture *fixture, const char *id, int64_t amount)
{
    UmiBankCommand c = SubmitCommand(fixture, id, amount); OK(Send(fixture, &maker, &c));
}
static void ExecuteTransfer(Fixture *fixture, const char *id, int64_t amount)
{
    Submit(fixture, id, amount); TransferAction(fixture, UMI_BANK_TRANSFER_APPROVE, id, &checker);
    TransferAction(fixture, UMI_BANK_TRANSFER_EXECUTE, id, &maker);
}
static void Balance(Fixture *fixture, const char *id, int64_t booked, int64_t reserved)
{
    UmiBankBalance balance; OK(UmiBankOperationsBalance(fixture->operations, id, &balance));
    CHECK(balance.booked.minor_units == booked); CHECK(balance.reserved.minor_units == reserved);
    CHECK(balance.available.minor_units == booked - reserved);
}
static void Setup(Fixture *fixture, bool fund)
{
    Customer(fixture, "c1"); Customer(fixture, "c2");
    Account(fixture, "a1", "c1", "GBP", 2U); Account(fixture, "a2", "c2", "GBP", 2U);
    Beneficiary(fixture, "b1", "c1", "a2"); if (fund) Credit(fixture, 100000);
}
static void Change(Fixture *fixture, UmiBankAction action, const char *id, UmiBankRecordState state)
{
    UmiBankCommand c = Command(fixture, action, id); c.state = state; OK(Send(fixture, &maker, &c));
}
static UmiBankCommand HoldCommand(Fixture *fixture, const char *id, int64_t amount)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_HOLD_PLACE, id);
    Id(&c.sourceAccountId, "a1"); c.amount = Money(amount, "GBP", 2U); return c;
}
static void IssueCard(Fixture *fixture)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_CARD_ISSUE, "card1");
    Id(&c.sourceAccountId, "a1"); strcpy(c.name, "Simulated card"); c.amount = Money(20000, "GBP", 2U);
    OK(Send(fixture, &maker, &c));
}
static UmiBankCommand AuthoriseCommand(Fixture *fixture, int64_t amount)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_CARD_AUTHORISE, "auth1");
    Id(&c.ownerId, "card1"); c.amount = Money(amount, "GBP", 2U); return c;
}
static void Authorise(Fixture *fixture, int64_t amount)
{
    UmiBankCommand c = AuthoriseCommand(fixture, amount); OK(Send(fixture, &maker, &c));
}
static UmiBankCommand CaptureCommand(Fixture *fixture, int64_t amount)
{
    UmiBankCommand c = Command(fixture, UMI_BANK_CARD_CAPTURE, "auth1");
    c.amount = Money(amount, "GBP", 2U); return c;
}

/* A single coherent workflow proves real state transitions, not feature flags. */
static void EndToEnd(Fixture *f)
{
    UmiBankCommand c; UmiBankJournal journal; UmiBankTransfer transfer;
    UmiBankStatement *statement = calloc(1U, sizeof *statement); UmiBankCounts counts;
    CHECK(statement != NULL); Setup(f, true); Submit(f, "t1", 25000); Balance(f, "a1", 100000, 25000);
    TransferAction(f, UMI_BANK_TRANSFER_APPROVE, "t1", &checker);
    TransferAction(f, UMI_BANK_TRANSFER_EXECUTE, "t1", &maker);
    Balance(f, "a1", 75000, 0); Balance(f, "a2", 25000, 0);
    OK(UmiBankOperationsTransferAt(f->operations, 0U, &transfer)); CHECK(transfer.state == UMI_BANK_TRANSFER_EXECUTED);
    OK(UmiBankOperationsJournalAt(f->operations, 1U, &journal));
    CHECK(umi_accounting_journal_entry_balanced(&journal.entry)); CHECK(journal.entry.status == UMI_ACCOUNTING_JOURNAL_POSTED);
    c = Command(f, UMI_BANK_RECONCILE, "r1"); Id(&c.sourceAccountId, "a1"); c.amount = Money(75000, "GBP", 2U);
    OK(Send(f, &maker, &c)); OK(UmiBankOperationsCounts(f->operations, &counts));
    OK(UmiBankOperationsStatement(f->operations, "a1", 1U, counts.revision, statement));
    CHECK(statement->count == 2U && statement->opening.minor_units == 0 && statement->closing.minor_units == 75000);
    OK(UmiBankOperationsReload(f->operations)); Balance(f, "a1", 75000, 0); free(statement);
}

static void DomainCase(Fixture *f, const char *name)
{
    UmiBankCommand c; UmiBankCounts before, after; UmiBankReceipt receipt;
    Setup(f, true); OK(UmiBankOperationsCounts(f->operations, &before));
    if (strcmp(name, "idempotent") == 0) {
        c = CreditCommand(f, 100); OK(Send(f, &maker, &c));
        OK(UmiBankOperationsExecute(f->operations, &maker, &c, &receipt)); CHECK(receipt.idempotent);
        Balance(f, "a1", 100100, 0); CHECK(receipt.revision == before.revision + 1U);
    } else if (strcmp(name, "idempotency-conflict") == 0) {
        c = CreditCommand(f, 100); OK(Send(f, &maker, &c)); c.amount.minor_units++;
        CHECK(Send(f, &maker, &c) == UMI_STATUS_ALREADY_EXISTS); Balance(f, "a1", 100100, 0);
    } else if (strcmp(name, "idempotency-actor") == 0) {
        c = CreditCommand(f, 100); OK(Send(f, &maker, &c));
        CHECK(Send(f, &checker, &c) == UMI_STATUS_ALREADY_EXISTS);
    } else if (strcmp(name, "idempotency-permission") == 0) {
        UmiBankActor denied = maker; denied.capabilities = 0U;
        c = CreditCommand(f, 100); OK(Send(f, &maker, &c));
        CHECK(Send(f, &denied, &c) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "stale-revision") == 0) {
        c = CreditCommand(f, 100); c.expectedRevision--;
        CHECK(Send(f, &maker, &c) == UMI_STATUS_BUSY); Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "insufficient-funds") == 0) {
        c = SubmitCommand(f, "t1", 100001); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
        Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "reservation-double-spend") == 0) {
        Submit(f, "t1", 75000); c = SubmitCommand(f, "t2", 25001);
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE); Balance(f, "a1", 100000, 75000);
    } else if (strcmp(name, "self-approval") == 0) {
        Submit(f, "t1", 100); c = Command(f, UMI_BANK_TRANSFER_APPROVE, "t1");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "unapproved-execution") == 0) {
        Submit(f, "t1", 100); c = Command(f, UMI_BANK_TRANSFER_EXECUTE, "t1");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "rejection") == 0) {
        Submit(f, "t1", 100); TransferAction(f, UMI_BANK_TRANSFER_REJECT, "t1", &checker);
        Balance(f, "a1", 100000, 0); c = Command(f, UMI_BANK_TRANSFER_APPROVE, "t1");
        CHECK(Send(f, &checker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "cancellation") == 0) {
        UmiBankActor operator = other; operator.capabilities = UMI_BANK_CAP_OPERATE;
        Submit(f, "t1", 100); c = Command(f, UMI_BANK_TRANSFER_CANCEL, "t1");
        CHECK(Send(f, &other, &c) == UMI_STATUS_PERMISSION_DENIED);
        OK(Send(f, &operator, &c)); Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "double-execution") == 0) {
        ExecuteTransfer(f, "t1", 100); c = Command(f, UMI_BANK_TRANSFER_EXECUTE, "t1");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE); Balance(f, "a1", 99900, 0);
    } else if (strcmp(name, "reversal") == 0) {
        ExecuteTransfer(f, "t1", 100); TransferAction(f, UMI_BANK_TRANSFER_REVERSE, "t1", &maker);
        Balance(f, "a1", 100000, 0); Balance(f, "a2", 0, 0);
        OK(UmiBankOperationsCounts(f->operations, &after)); CHECK(after.journals == 3U);
        { UmiBankJournal j; OK(UmiBankOperationsJournalAt(f->operations, 2U, &j)); CHECK(j.reversal); }
    } else if (strcmp(name, "reversal-reserved") == 0) {
        ExecuteTransfer(f, "t1", 100); c = HoldCommand(f, "h2", 1); Id(&c.sourceAccountId, "a2");
        OK(Send(f, &maker, &c)); c = Command(f, UMI_BANK_TRANSFER_REVERSE, "t1");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE); Balance(f, "a2", 100, 1);
    } else if (strcmp(name, "currency-mismatch") == 0) {
        c = SubmitCommand(f, "t1", 100); c.amount = Money(100, "USD", 2U);
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "currency-scales") == 0) {
        Account(f, "yen", "c1", "JPY", 0U); Account(f, "kwd", "c1", "KWD", 3U);
        c = Command(f, UMI_BANK_ACCOUNT_OPEN, "bad"); Id(&c.ownerId, "c1"); strcpy(c.name, "Invalid scale");
        c.amount = Money(0, "GBP", 3U); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "account-block") == 0) {
        Change(f, UMI_BANK_ACCOUNT_SET_STATE, "a1", UMI_BANK_RECORD_BLOCKED);
        c = SubmitCommand(f, "t1", 100); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "customer-block") == 0) {
        Change(f, UMI_BANK_CUSTOMER_SET_STATE, "c1", UMI_BANK_RECORD_BLOCKED);
        c = SubmitCommand(f, "t1", 100); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "beneficiary-block") == 0) {
        Submit(f, "t1", 100); TransferAction(f, UMI_BANK_TRANSFER_APPROVE, "t1", &checker);
        Change(f, UMI_BANK_BENEFICIARY_SET_STATE, "b1", UMI_BANK_RECORD_BLOCKED);
        c = Command(f, UMI_BANK_TRANSFER_EXECUTE, "t1"); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "beneficiary-owner") == 0) {
        Beneficiary(f, "b2", "c2", "a2"); c = SubmitCommand(f, "t1", 100); Id(&c.ownerId, "b2");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "account-close") == 0) {
        c = Command(f, UMI_BANK_ACCOUNT_SET_STATE, "a1"); c.state = UMI_BANK_RECORD_CLOSED;
        CHECK(Send(f, &maker, &c) == UMI_STATUS_BUSY);
        Change(f, UMI_BANK_ACCOUNT_SET_STATE, "a2", UMI_BANK_RECORD_CLOSED);
        c = Command(f, UMI_BANK_ACCOUNT_SET_STATE, "a2"); c.state = UMI_BANK_RECORD_ACTIVE;
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "close-pending-recipient") == 0) {
        Submit(f, "t1", 100); c = Command(f, UMI_BANK_ACCOUNT_SET_STATE, "a2"); c.state = UMI_BANK_RECORD_CLOSED;
        CHECK(Send(f, &maker, &c) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "manual-hold") == 0) {
        c = HoldCommand(f, "h1", 50000); OK(Send(f, &maker, &c)); Balance(f, "a1", 100000, 50000);
        TransferAction(f, UMI_BANK_HOLD_RELEASE, "h1", &maker); Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "hold-overflow") == 0) {
        c = HoldCommand(f, "h1", INT64_MAX); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "credit-overflow") == 0) {
        c = CreditCommand(f, INT64_MAX); CHECK(Send(f, &maker, &c) == UMI_STATUS_CAPACITY_EXCEEDED);
        Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "credit-permission") == 0) {
        c = CreditCommand(f, 1); CHECK(Send(f, &other, &c) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "statement-range") == 0) {
        UmiBankStatement *s = calloc(1U, sizeof *s); CHECK(s != NULL); ExecuteTransfer(f, "t1", 100);
        OK(UmiBankOperationsCounts(f->operations, &after));
        OK(UmiBankOperationsStatement(f->operations, "a1", before.revision + 1U, after.revision, s));
        CHECK(s->opening.minor_units == 100000 && s->closing.minor_units == 99900 && s->count == 1U);
        CHECK(UmiBankOperationsStatement(f->operations, "a1", 0U, after.revision, s) == UMI_STATUS_INVALID_ARGUMENT); free(s);
    } else if (strcmp(name, "reconciliation-break") == 0) {
        UmiBankReconciliation r; c = Command(f, UMI_BANK_RECONCILE, "r1"); Id(&c.sourceAccountId, "a1");
        c.amount = Money(99999, "GBP", 2U); OK(Send(f, &maker, &c));
        OK(UmiBankOperationsReconciliationAt(f->operations, 0U, &r)); CHECK(!r.matched);
        CHECK(r.bookedBalance.minor_units == 100000); Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "copy-isolation") == 0) {
        UmiBankAccount account; OK(UmiBankOperationsAccountAt(f->operations, 0U, &account));
        account.state = UMI_BANK_RECORD_CLOSED; OK(UmiBankOperationsAccountAt(f->operations, 0U, &account));
        CHECK(account.state == UMI_BANK_RECORD_ACTIVE);
        CHECK(UmiBankOperationsAccountAt(f->operations, 999U, &account) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "unused-command-field") == 0) {
        c = Command(f, UMI_BANK_TRANSFER_REVERSE, "missing"); c.amount = Money(100, "GBP", 2U);
        CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "recipient-overflow") == 0) {
        c = CreditCommand(f, INT64_MAX); Id(&c.sourceAccountId, "a2"); OK(Send(f, &maker, &c));
        Submit(f, "t1", 1); TransferAction(f, UMI_BANK_TRANSFER_APPROVE, "t1", &checker);
        c = Command(f, UMI_BANK_TRANSFER_EXECUTE, "t1"); CHECK(Send(f, &maker, &c) == UMI_STATUS_CAPACITY_EXCEEDED);
        Balance(f, "a1", 100000, 1); Balance(f, "a2", INT64_MAX, 0);
    } else if (strcmp(name, "audit-lineage") == 0) {
        UmiBankAuditEvent event; Submit(f, "t1", 100); TransferAction(f, UMI_BANK_TRANSFER_APPROVE, "t1", &checker);
        OK(UmiBankOperationsAuditAt(f->operations, (size_t)before.revision + 1U, &event));
        CHECK(strcmp(event.actor.id.value, checker.id.value) == 0 && event.command.action == UMI_BANK_TRANSFER_APPROVE);
    } else CHECK(false);
}

static void CardCase(Fixture *f, const char *name)
{
    UmiBankCommand c; Setup(f, true); IssueCard(f);
    if (strcmp(name, "card-limit") == 0) {
        c = AuthoriseCommand(f, 20001); CHECK(Send(f, &maker, &c) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "card-freeze") == 0) {
        Change(f, UMI_BANK_CARD_SET_STATE, "card1", UMI_BANK_RECORD_BLOCKED);
        c = AuthoriseCommand(f, 100); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
    } else {
        Authorise(f, 10000); Balance(f, "a1", 100000, 10000);
        if (strcmp(name, "card-void") == 0) {
            TransferAction(f, UMI_BANK_CARD_VOID, "auth1", &maker); Balance(f, "a1", 100000, 0);
        } else if (strcmp(name, "card-close-reserved") == 0) {
            c = Command(f, UMI_BANK_CARD_SET_STATE, "card1"); c.state = UMI_BANK_RECORD_CLOSED;
            CHECK(Send(f, &maker, &c) == UMI_STATUS_BUSY);
        } else if (strcmp(name, "card-overcapture") == 0) {
            c = CaptureCommand(f, 10001); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_ARGUMENT);
            Balance(f, "a1", 100000, 10000);
        } else if (strcmp(name, "card-manual-release") == 0) {
            c = Command(f, UMI_BANK_HOLD_RELEASE, "auth1"); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
        } else {
            c = CaptureCommand(f, 7000); OK(Send(f, &maker, &c)); Balance(f, "a1", 93000, 0);
            if (strcmp(name, "card-capture") == 0) {
                c = CaptureCommand(f, 1000); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
            } else if (strcmp(name, "card-refund") == 0) {
                TransferAction(f, UMI_BANK_CARD_REFUND, "auth1", &maker); Balance(f, "a1", 100000, 0);
                c = Command(f, UMI_BANK_CARD_REFUND, "auth1"); CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_STATE);
            } else CHECK(false);
        }
    }
}

static void ValidationCase(Fixture *f, const char *name)
{
    UmiBankCommand c = Command(f, UMI_BANK_CUSTOMER_CREATE, "c1"); UmiBankCounts counts;
    strcpy(c.name, "Customer");
    if (strcmp(name, "invalid-id") == 0) memset(c.id.value, 'x', sizeof c.id.value);
    else if (strcmp(name, "reserved-id") == 0) strcpy(c.id.value, "sys.funding.GBP");
    else if (strcmp(name, "invalid-date") == 0) c.businessDate = (UmiFinancialDate){2026, 2U, 29U};
    else if (strcmp(name, "negative-money") == 0) c.amount.minor_units = -1;
    else if (strcmp(name, "invalid-name") == 0) memset(c.name, 'x', sizeof c.name);
    else if (strcmp(name, "invalid-state") == 0) c.state = (UmiBankRecordState)99;
    else if (strcmp(name, "invalid-currency") == 0) memcpy(c.amount.currency.code, "GBPX", 4U);
    else if (strcmp(name, "invalid-scale") == 0) c.amount = Money(1, "GBP", 10U);
    else CHECK(false);
    CHECK(Send(f, &maker, &c) == UMI_STATUS_INVALID_ARGUMENT);
    OK(UmiBankOperationsCounts(f->operations, &counts)); CHECK(counts.revision == 0U);
}
static void CapacityCase(Fixture *f, const char *name)
{
    char id[48]; UmiBankCommand c;
    if (strcmp(name, "customer-capacity") == 0) {
        for (size_t i = 0U; i < UMI_BANK_RECORD_CAPACITY; ++i) {
            (void)snprintf(id, sizeof id, "customer-%zu", i); Customer(f, id);
        }
        c = Command(f, UMI_BANK_CUSTOMER_CREATE, "overflow"); strcpy(c.name, "Overflow");
        CHECK(Send(f, &maker, &c) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else {
        Customer(f, "c1"); Account(f, "a1", "c1", "GBP", 2U);
        for (size_t i = 2U; i < UMI_BANK_EVENT_CAPACITY; ++i) Credit(f, 1);
        c = CreditCommand(f, 1); CHECK(Send(f, &maker, &c) == UMI_STATUS_CAPACITY_EXCEEDED);
        Balance(f, "a1", (int64_t)UMI_BANK_EVENT_CAPACITY - 2, 0);
    }
    OK(UmiBankOperationsReload(f->operations));
}
static void CodecCase(Fixture *f, const char *name)
{
    UmiBankAuditEvent original, decoded; char text[BANK_RECORD_TEXT_CAPACITY];
    Customer(f, "c1"); OK(UmiBankOperationsAuditAt(f->operations, 0U, &original));
    OK(BankEncode(&original, text, sizeof text));
    if (strcmp(name, "codec-roundtrip") == 0) {
        OK(BankDecode(text, &decoded)); CHECK(BankSameRequest(&decoded, &original.actor, &original.command));
        CHECK(BankEncode(&original, text, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(name, "codec-truncation") == 0) {
        size_t length = strlen(text);
        for (size_t i = 0U; i < length; ++i) {
            char saved = text[i]; text[i] = '\0'; CHECK(BankDecode(text, &decoded) != UMI_STATUS_OK); text[i] = saved;
        }
    } else if (strcmp(name, "codec-version") == 0) {
        text[0] = '0'; CHECK(BankDecode(text, &decoded) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "codec-trailing") == 0) {
        strcat(text, "00"); CHECK(BankDecode(text, &decoded) == UMI_STATUS_PARSE_ERROR);
    } else CHECK(false);
}
static UmiStatus VisitCount(const char *key, const char *value, void *data)
{
    size_t *count = data; (void)key; (void)value; (*count)++; return UMI_STATUS_OK;
}

static void PersistenceCase(Fixture *f, const char *name, const char *path)
{
    UmiBankCommand c; UmiBankCounts before, after; char text[4096]; UmiBankReceipt receipt;
    Setup(f, true); OK(UmiBankOperationsCounts(f->operations, &before));
    if (strcmp(name, "sqlite-restart") == 0) {
        c = CreditCommand(f, 123); OK(Send(f, &maker, &c));
        UmiBankOperationsDestroy(f->operations); f->operations = NULL;
        OK(UmiBankOperationsOpenSqlite(path, &f->operations));
        OK(UmiBankOperationsExecute(f->operations, &maker, &c, &receipt)); CHECK(receipt.idempotent);
        Balance(f, "a1", 100123, 0);
    } else if (strcmp(name, "sqlite-stale-writer") == 0) {
        UmiBankOperations *second = NULL; OK(UmiBankOperationsOpenSqlite(path, &second));
        Credit(f, 10); c = CreditCommand(f, 20); c.expectedRevision = before.revision;
        CHECK(UmiBankOperationsExecute(second, &maker, &c, &receipt) == UMI_STATUS_BUSY);
        OK(UmiBankOperationsReload(second)); c.expectedRevision++;
        OK(UmiBankOperationsExecute(second, &maker, &c, &receipt)); UmiBankOperationsDestroy(second);
        OK(UmiBankOperationsReload(f->operations)); Balance(f, "a1", 100030, 0);
    } else if (strcmp(name, "sqlite-write-failure") == 0 || strcmp(name, "sqlite-commit-failure") == 0) {
        if (strcmp(name, "sqlite-write-failure") == 0) {
            OK(umi_data_server_execute(f->operations->server,
                "CREATE TRIGGER bank_test_failure BEFORE UPDATE ON umicom_kv "
                "WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ABORT,'injected'); END;"));
        } else {
            OK(umi_data_server_execute(f->operations->server,
                "PRAGMA foreign_keys=ON; CREATE TABLE bank_test_parent(id INTEGER PRIMARY KEY);"
                "CREATE TABLE bank_test_child(id INTEGER REFERENCES bank_test_parent(id) DEFERRABLE INITIALLY DEFERRED);"
                "CREATE TRIGGER bank_test_failure AFTER INSERT ON umicom_kv "
                "WHEN NEW.key LIKE 'bank.operations.event.%' BEGIN INSERT INTO bank_test_child VALUES(7); END;"));
        }
        c = CreditCommand(f, 123); CHECK(Send(f, &maker, &c) == UMI_STATUS_IO_ERROR);
        Balance(f, "a1", 100000, 0); OK(UmiBankOperationsCounts(f->operations, &after)); CHECK(after.revision == before.revision);
        CHECK(umi_data_server_count(f->operations->server) == before.events + 1U);
        OK(umi_data_server_execute(f->operations->server, "DROP TRIGGER bank_test_failure;"));
        OK(UmiBankOperationsReload(f->operations)); OK(Send(f, &maker, &c)); Balance(f, "a1", 100123, 0);
    } else if (strcmp(name, "sqlite-rollback-poison") == 0) {
        OK(umi_data_server_execute(f->operations->server,
            "CREATE TRIGGER bank_test_failure BEFORE UPDATE ON umicom_kv "
            "WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ROLLBACK,'injected rollback'); END;"));
        c = CreditCommand(f, 123); CHECK(Send(f, &maker, &c) == UMI_STATUS_IO_ERROR);
        CHECK(UmiBankOperationsCounts(f->operations, &after) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiBankOperationsReload(f->operations) == UMI_STATUS_INVALID_STATE);
        UmiBankOperationsDestroy(f->operations); f->operations = NULL;
        OK(UmiBankOperationsOpenSqlite(path, &f->operations)); Balance(f, "a1", 100000, 0);
    } else if (strcmp(name, "sqlite-corrupt-event") == 0 || strcmp(name, "sqlite-missing-event") == 0 ||
               strcmp(name, "sqlite-orphan-event") == 0 || strcmp(name, "sqlite-unknown-schema") == 0) {
        if (strcmp(name, "sqlite-corrupt-event") == 0)
            OK(umi_data_server_set(f->operations->server, BANK_EVENT_KEY_PREFIX "00000000000000000001", "broken"));
        else if (strcmp(name, "sqlite-missing-event") == 0)
            OK(umi_data_server_delete(f->operations->server, BANK_EVENT_KEY_PREFIX "00000000000000000001"));
        else if (strcmp(name, "sqlite-orphan-event") == 0)
            OK(umi_data_server_set(f->operations->server, BANK_EVENT_KEY_PREFIX "00000000000000000256", "orphan"));
        else OK(umi_data_server_set(f->operations->server, BANK_REVISION_KEY, "UBANK99:6"));
        CHECK(UmiBankOperationsReload(f->operations) == UMI_STATUS_PARSE_ERROR);
        Balance(f, "a1", 100000, 0); /* Failed reload never publishes a partial projection. */
        UmiBankOperationsDestroy(f->operations); f->operations = NULL;
        CHECK(UmiBankOperationsOpenSqlite(path, &f->operations) == UMI_STATUS_PARSE_ERROR); CHECK(f->operations == NULL);
    } else if (strcmp(name, "sqlite-read-error") == 0 || strcmp(name, "sqlite-enumeration-error") == 0) {
        OK(umi_data_server_execute(f->operations->server,
            "ALTER TABLE umicom_kv RENAME TO bank_test_original;"
            "CREATE VIEW umicom_kv AS SELECT key,abs(-9223372036854775808) AS value FROM bank_test_original;"));
        if (strcmp(name, "sqlite-read-error") == 0)
            CHECK(umi_data_server_get(f->operations->server, BANK_REVISION_KEY, text, sizeof text) == UMI_STATUS_IO_ERROR);
        else { size_t count = 0U; CHECK(umi_data_server_visit(f->operations->server, VisitCount, &count) == UMI_STATUS_IO_ERROR); }
    } else if (strcmp(name, "sqlite-embedded-nul") == 0) {
        size_t count = 0U;
        OK(umi_data_server_execute(f->operations->server, "UPDATE umicom_kv SET value='UBANK1:6'||char(0)||'hidden' WHERE key='bank.operations.revision';"));
        CHECK(umi_data_server_get(f->operations->server, BANK_REVISION_KEY, text, sizeof text) == UMI_STATUS_PARSE_ERROR);
        CHECK(umi_data_server_visit(f->operations->server, VisitCount, &count) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiBankOperationsReload(f->operations) == UMI_STATUS_PARSE_ERROR);
    } else CHECK(false);
}

int main(int argc, char **argv)
{
    Fixture fixture = {NULL, 0U}; const char *name;
    CHECK(argc >= 2); name = argv[1]; Id(&maker.id, "test-maker"); Id(&checker.id, "test-checker"); Id(&other.id, "test-other");
    if (strncmp(name, "sqlite-", 7U) == 0) {
        UmiStatus status; CHECK(argc == 3); (void)remove(argv[2]);
        status = UmiBankOperationsOpenSqlite(argv[2], &fixture.operations);
        if (status == UMI_STATUS_UNAVAILABLE) { puts("SKIP: Framework built without SQLite"); return 77; }
        OK(status); PersistenceCase(&fixture, name, argv[2]);
    } else {
        OK(UmiBankOperationsOpenMemory(&fixture.operations));
        if (strcmp(name, "end-to-end") == 0) EndToEnd(&fixture);
        else if (strncmp(name, "card-", 5U) == 0) CardCase(&fixture, name);
        else if (strncmp(name, "invalid-", 8U) == 0 || strcmp(name, "negative-money") == 0 || strcmp(name, "reserved-id") == 0)
            ValidationCase(&fixture, name);
        else if (strstr(name, "capacity") != NULL) CapacityCase(&fixture, name);
        else if (strncmp(name, "codec-", 6U) == 0) CodecCase(&fixture, name);
        else if (strcmp(name, "absolute-path") == 0) {
            UmiBankOperations *bad = fixture.operations;
            CHECK(UmiBankOperationsOpenSqlite("relative.sqlite", &bad) == UMI_STATUS_INVALID_ARGUMENT); CHECK(bad == NULL);
        } else DomainCase(&fixture, name);
    }
    UmiBankOperationsDestroy(fixture.operations);
    if (strncmp(name, "sqlite-", 7U) == 0) (void)remove(argv[2]);
    (void)printf("PASS %s\n", name); return 0;
}
