/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_operations/test_operations.c
 *
 * PURPOSE:
 *   Exercise accounting, price/time matching, atomic settlement, persistence and rejected-command invariants through real Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_operations/operations.h"
#include "umicom/finance_operations/input.h"
#include "../../src/finance_operations/internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(1); } } while (0)
#define OK(expression) CHECK((expression) == UMI_STATUS_OK)

typedef struct Fixture {
    UmiDataServer *server;
    UmiFinanceOperations *operations;
    UmiFinanceOperationCommand command;
    UmiFinanceOperationReceipt receipt;
    unsigned request;
} Fixture;
static const UmiCurrency GBP = {{'G','B','P','\0'}};
static const UmiCurrency USD = {{'U','S','D','\0'}};
static const UmiFinancialDate DAY = {2026, 9U, 25U};

static uint64_t Revision(Fixture *fixture)
{
    UmiFinanceOperationCounts counts;
    OK(UmiFinanceOperationsCounts(fixture->operations, &counts));
    return counts.revision;
}
static void Id(UmiFinancialId *id, const char *text) { OK(FinanceSetId(id, text)); }
static void Begin(Fixture *fixture, UmiFinanceOperationKind kind, const char *id, const char *actor)
{
    char request[48];
    UmiFinanceOperationCommandInit(&fixture->command);
    fixture->command.kind = kind;
    fixture->command.expectedRevision = Revision(fixture);
    (void)snprintf(request, sizeof(request), "request.%u", ++fixture->request);
    Id(&fixture->command.requestId, request);
    Id(&fixture->command.actorId, actor);
    Id(&fixture->command.id, id);
}
static void Apply(Fixture *fixture) { OK(UmiFinanceOperationsApply(fixture->operations, &fixture->command, &fixture->receipt)); }
static void Reject(Fixture *fixture, UmiStatus expected)
{
    uint64_t before = Revision(fixture);
    CHECK(UmiFinanceOperationsApply(fixture->operations, &fixture->command, &fixture->receipt) == expected);
    CHECK(Revision(fixture) == before);
}
static void Create(Fixture *fixture, const char *path)
{
    memset(fixture, 0, sizeof(*fixture));
    if (path == NULL) OK(umi_data_server_create_memory(&fixture->server));
    else OK(umi_data_server_create_sqlite(path, &fixture->server));
    OK(UmiFinanceOperationsCreate(fixture->server, &fixture->operations));
}
static void Destroy(Fixture *fixture)
{
    UmiFinanceOperationsDestroy(fixture->operations);
    umi_data_server_destroy(fixture->server);
    fixture->operations = NULL; fixture->server = NULL;
}
static void Account(Fixture *fixture, const char *id, UmiAccountingAccountClass accountClass, UmiCurrency currency)
{
    Begin(fixture, UMI_FINANCE_CREATE_ACCOUNT, id, "operator");
    (void)snprintf(fixture->command.name, sizeof(fixture->command.name), "%s account", id);
    fixture->command.accountClass = accountClass;
    fixture->command.currency = currency; fixture->command.scale = 2U;
    Apply(fixture);
}
static void Period(Fixture *fixture, const char *id, uint8_t month)
{
    Begin(fixture, UMI_FINANCE_OPEN_PERIOD, id, "operator");
    fixture->command.date = (UmiFinancialDate){2026, month, 1U};
    fixture->command.endDate = (UmiFinancialDate){2026, month, month == 9U ? 30U : 31U};
    Apply(fixture);
}
static void Journal(Fixture *fixture, const char *id, const char *debit, const char *credit, int64_t amount)
{
    Begin(fixture, UMI_FINANCE_ENTER_JOURNAL, id, "maker");
    Id(&fixture->command.referenceId, "september"); fixture->command.date = DAY;
    fixture->command.lineCount = 2U;
    Id(&fixture->command.lines[0].accountId, debit); fixture->command.lines[0].debitMinor = amount;
    Id(&fixture->command.lines[1].accountId, credit); fixture->command.lines[1].creditMinor = amount;
    Apply(fixture);
}
static void ApprovePost(Fixture *fixture, const char *id)
{
    Begin(fixture, UMI_FINANCE_APPROVE_JOURNAL, id, "checker"); Apply(fixture);
    Begin(fixture, UMI_FINANCE_POST_JOURNAL, id, "checker"); Apply(fixture);
}
static void Participant(Fixture *fixture, const char *id, const char *cash)
{
    Begin(fixture, UMI_FINANCE_REGISTER_PARTICIPANT, id, "operator");
    Id(&fixture->command.accountId, cash);
    (void)snprintf(fixture->command.name, sizeof(fixture->command.name), "%s participant", id);
    Apply(fixture);
}
static void Instrument(Fixture *fixture, const char *id)
{
    Begin(fixture, UMI_FINANCE_LIST_INSTRUMENT, id, "operator");
    (void)snprintf(fixture->command.name, sizeof(fixture->command.name), "%s instrument", id);
    fixture->command.currency = GBP; fixture->command.scale = 2U;
    fixture->command.minorPerTick = 1; fixture->command.unitsPerLot = 1; fixture->command.maxOrderLots = 1000;
    Apply(fixture);
    Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, id, "operator"); fixture->command.enabled = true; Apply(fixture);
}
static void Inventory(Fixture *fixture, const char *participant, int64_t lots)
{
    Begin(fixture, UMI_FINANCE_DEPOSIT_LOTS, participant, "custodian");
    Id(&fixture->command.instrumentId, "practice"); fixture->command.lots = lots; Apply(fixture);
}
static void OrderCommand(Fixture *fixture, const char *id, const char *participant, UmiSide side, int64_t price, int64_t lots)
{
    Begin(fixture, UMI_FINANCE_PLACE_ORDER, id, "trader");
    Id(&fixture->command.participantId, participant); Id(&fixture->command.instrumentId, "practice");
    fixture->command.side = side; fixture->command.priceTicks = price; fixture->command.lots = lots; fixture->command.date = DAY;
}
static void Order(Fixture *fixture, const char *id, const char *participant, UmiSide side, int64_t price, int64_t lots)
{ OrderCommand(fixture, id, participant, side, price, lots); Apply(fixture); }
static void Cancel(Fixture *fixture, const char *id, const char *participant)
{
    Begin(fixture, UMI_FINANCE_CANCEL_ORDER, id, "trader"); Id(&fixture->command.participantId, participant); Apply(fixture);
}
static void Settle(Fixture *fixture, const char *id)
{
    Begin(fixture, UMI_FINANCE_CLEAR_FILL, id, "clearing"); Apply(fixture);
    Begin(fixture, UMI_FINANCE_SETTLE_FILL, id, "settlement");
    Id(&fixture->command.periodId, "september"); fixture->command.date = DAY; Apply(fixture);
}
static void Setup(Fixture *fixture)
{
    Account(fixture, "control", UMI_ACCOUNTING_ASSET, GBP);
    Account(fixture, "buyer.cash", UMI_ACCOUNTING_LIABILITY, GBP);
    Account(fixture, "seller.cash", UMI_ACCOUNTING_LIABILITY, GBP);
    Period(fixture, "september", 9U);
    Journal(fixture, "funding", "control", "buyer.cash", 50000); ApprovePost(fixture, "funding");
    Participant(fixture, "buyer", "buyer.cash"); Participant(fixture, "seller", "seller.cash");
    Instrument(fixture, "practice"); Inventory(fixture, "seller", 100);
}
static void Match(Fixture *fixture)
{
    Order(fixture, "sell.1", "seller", UMI_SIDE_SELL, 1000, 10);
    Order(fixture, "buy.1", "buyer", UMI_SIDE_BUY, 1100, 6);
}
static void Reconcile(Fixture *fixture, const char *id, const char *accountId, int64_t amount)
{
    Begin(fixture, UMI_FINANCE_RECONCILE_ACCOUNT, id, "reconciler");
    Id(&fixture->command.accountId, accountId); fixture->command.amountMinor = amount; Apply(fixture);
}
static void ReconcileAll(Fixture *fixture)
{
    int64_t amount;
    const char *ids[] = {"control", "buyer.cash", "seller.cash"};
    for (size_t index = 0U; index < 3U; ++index) {
        char evidence[48]; (void)snprintf(evidence, sizeof(evidence), "recon.%u.%zu", fixture->request, index);
        OK(UmiFinanceOperationsAccountBalance(fixture->operations, ids[index], &amount));
        Reconcile(fixture, evidence, ids[index], amount);
    }
}
static void CheckBalances(Fixture *fixture, int64_t buyer, int64_t seller)
{
    int64_t value; UmiFinanceTrialBalance report;
    OK(UmiFinanceOperationsAccountBalance(fixture->operations, "buyer.cash", &value)); CHECK(value == buyer);
    OK(UmiFinanceOperationsAccountBalance(fixture->operations, "seller.cash", &value)); CHECK(value == seller);
    OK(UmiFinanceOperationsTrialBalance(fixture->operations, "september", GBP, 2U, &report)); CHECK(report.balanced);
}
static void TestCase(const char *name, Fixture *fixture)
{
    UmiFinanceOperationCommand saved;
    UmiFinanceOperationAvailability availability;
    UmiFinanceOperationCounts counts;
    UmiFinanceOperationFill fill;
    UmiFinanceOperationOrder order;
    UmiFinanceOperationJournal journal;
    UmiFinanceOperationPeriod period;
    UmiFinanceTrialBalance report;
    uint64_t before;
    int64_t value;
    if (strcmp(name, "empty") == 0) {
        CHECK(Revision(fixture) == 0U); CHECK(umi_data_server_count(fixture->server) == 0U);
        CHECK(UmiFinanceOperationsAccountAt(fixture->operations, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return;
    }
    if (strcmp(name, "format") == 0) {
        char buffer[64];
        OK(UmiFinanceOperationsFormatAmount(INT64_MIN, 2U, buffer, sizeof(buffer)));
        CHECK(strcmp(buffer, "-92233720368547758.08") == 0);
        OK(UmiFinanceOperationsFormatAmount(7, 3U, buffer, sizeof(buffer))); CHECK(strcmp(buffer, "0.007") == 0);
        CHECK(UmiFinanceOperationsFormatAmount(123, 2U, buffer, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(buffer[0] == '\0');
        return;
    }
    if (strcmp(name, "input_integer") == 0) {
        const char *bad[] = {"", "1.5", "1e2", "+1", "1,000", "--1", "1x", "- 1"};
        OK(UmiFinanceOperationsParseInteger(" -9223372036854775808 ", &value)); CHECK(value == INT64_MIN);
        for (size_t index = 0U; index < sizeof(bad)/sizeof(bad[0]); ++index)
            CHECK(UmiFinanceOperationsParseInteger(bad[index], &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceOperationsParseInteger("9223372036854775808", &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        return;
    }
    if (strcmp(name, "input_date") == 0) {
        UmiFinancialDate date;
        OK(UmiFinanceOperationsParseDate("2000-02-29", &date));
        CHECK(UmiFinanceOperationsParseDate("1900-02-29", &date) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceOperationsParseDate("2026-9-25", &date) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceOperationsParseDate("2026-09-25x", &date) == UMI_STATUS_INVALID_ARGUMENT); return;
    }
    if (strcmp(name, "input_lines") == 0) {
        UmiFinanceOperationLine lines[8] = {0}; size_t count = 999U;
        OK(UmiFinanceOperationsParseLines(" control , 100 , 0\r\n\nbuyer.cash,0,100\n", lines, 8U, &count));
        CHECK(count == 2U && lines[0].debitMinor == 100);
        CHECK(UmiFinanceOperationsParseLines("a,100,1\nb,0,100", lines, 8U, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(count == 0U);
        CHECK(UmiFinanceOperationsParseLines("a,100\nb,0,100", lines, 8U, &count) == UMI_STATUS_PARSE_ERROR); return;
    }
    Setup(fixture);
    if (strcmp(name, "funding") == 0) { CheckBalances(fixture, 50000, 0); return; }
    if (strcmp(name, "duplicate") == 0) {
        saved = fixture->command; before = Revision(fixture);
        saved.expectedRevision = before; OK(UmiFinanceOperationsApply(fixture->operations, &saved, &fixture->receipt));
        CHECK(fixture->receipt.duplicate && fixture->receipt.revision == before && Revision(fixture) == before);
        saved.lots = 999; CHECK(UmiFinanceOperationsApply(fixture->operations, &saved, &fixture->receipt) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(Revision(fixture) == before); return;
    }
    if (strcmp(name, "stale") == 0) {
        Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); --fixture->command.expectedRevision;
        Reject(fixture, UMI_STATUS_BUSY); return;
    }
    if (strcmp(name, "codec") == 0) {
        char encoded[FINANCE_RECORD_CAPACITY]; UmiFinanceOperationCommand decoded;
        OK(FinanceEncode(&fixture->command, encoded, sizeof(encoded)));
        OK(FinanceDecode(encoded, &decoded)); CHECK(FinanceSameCommand(&decoded, &fixture->command));
        encoded[5] = '2'; CHECK(FinanceDecode(encoded, &decoded) == UMI_STATUS_PARSE_ERROR);
        return;
    }
    if (strcmp(name, "bounds") == 0) {
        saved = fixture->command;
        memset(fixture->command.id.value, 'A', sizeof(fixture->command.id.value)); Reject(fixture, UMI_STATUS_INVALID_ARGUMENT);
        fixture->command = saved; fixture->command.lineCount = 9U; Reject(fixture, UMI_STATUS_INVALID_ARGUMENT);
        fixture->command = saved; Id(&fixture->command.id, "system.settlement.000001"); Reject(fixture, UMI_STATUS_INVALID_ARGUMENT);
        return;
    }
    if (strcmp(name, "maker_checker") == 0) {
        Journal(fixture, "extra", "control", "buyer.cash", 100);
        Begin(fixture, UMI_FINANCE_APPROVE_JOURNAL, "extra", "maker"); Reject(fixture, UMI_STATUS_PERMISSION_DENIED);
        Begin(fixture, UMI_FINANCE_POST_JOURNAL, "extra", "checker"); Reject(fixture, UMI_STATUS_PERMISSION_DENIED);
        Begin(fixture, UMI_FINANCE_APPROVE_JOURNAL, "extra", "checker"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_POST_JOURNAL, "extra", "maker"); Reject(fixture, UMI_STATUS_PERMISSION_DENIED);
        Begin(fixture, UMI_FINANCE_POST_JOURNAL, "extra", "checker"); Apply(fixture);
        CheckBalances(fixture, 50100, 0); return;
    }
    if (strcmp(name, "unbalanced") == 0) {
        Journal(fixture, "draft", "control", "buyer.cash", 100);
        saved = fixture->command; Begin(fixture, UMI_FINANCE_ENTER_JOURNAL, "bad", "maker");
        fixture->command.lineCount = 2U; fixture->command.lines[0] = saved.lines[0]; fixture->command.lines[1] = saved.lines[1];
        --fixture->command.lines[1].creditMinor; fixture->command.date = DAY; Id(&fixture->command.referenceId, "september");
        Reject(fixture, UMI_STATUS_INVALID_ARGUMENT); return;
    }
    if (strcmp(name, "cross_currency") == 0) {
        Account(fixture, "dollar", UMI_ACCOUNTING_ASSET, USD);
        Begin(fixture, UMI_FINANCE_ENTER_JOURNAL, "bad.fx", "maker"); Id(&fixture->command.referenceId, "september");
        fixture->command.date = DAY; fixture->command.lineCount = 2U;
        Id(&fixture->command.lines[0].accountId, "dollar"); fixture->command.lines[0].debitMinor = 100;
        Id(&fixture->command.lines[1].accountId, "buyer.cash"); fixture->command.lines[1].creditMinor = 100;
        Reject(fixture, UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiFinanceOperationsTrialBalance(fixture->operations, "september", USD, 2U, &report));
        CHECK(report.balanced && report.periodDebitMinor == 0); return;
    }
    if (strcmp(name, "currency_scale") == 0) {
        Begin(fixture, UMI_FINANCE_CREATE_ACCOUNT, "bad.scale", "operator"); strcpy(fixture->command.name, "Bad scale");
        fixture->command.currency = GBP; fixture->command.scale = 3U;
        Reject(fixture, UMI_STATUS_INVALID_ARGUMENT); return;
    }
    if (strcmp(name, "period_overlap") == 0) {
        Begin(fixture, UMI_FINANCE_OPEN_PERIOD, "overlap", "operator"); fixture->command.date = DAY;
        fixture->command.endDate = (UmiFinancialDate){2026,10U,1U}; Reject(fixture, UMI_STATUS_INVALID_ARGUMENT); return;
    }
    if (strcmp(name, "buying_power") == 0) {
        OrderCommand(fixture, "large", "buyer", UMI_SIDE_BUY, 1000, 51); Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    if (strcmp(name, "oversell") == 0) {
        OrderCommand(fixture, "large", "seller", UMI_SIDE_SELL, 1000, 101); Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    if (strcmp(name, "price_overflow") == 0) {
        OrderCommand(fixture, "overflow", "buyer", UMI_SIDE_BUY, INT64_MAX, 2); Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED); return;
    }
    if (strcmp(name, "order_limit") == 0) {
        OrderCommand(fixture, "limit", "buyer", UMI_SIDE_BUY, 1, 1001); Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    if (strcmp(name, "halt") == 0) {
        Order(fixture, "resting", "seller", UMI_SIDE_SELL, 1000, 10);
        Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); Apply(fixture);
        OrderCommand(fixture, "halted", "buyer", UMI_SIDE_BUY, 1000, 1); Reject(fixture, UMI_STATUS_INVALID_STATE);
        Cancel(fixture, "resting", "seller"); return;
    }
    if (strcmp(name, "cash_reservation") == 0) {
        Order(fixture, "reserve", "buyer", UMI_SIDE_BUY, 1200, 10);
        OK(UmiFinanceOperationsCash(fixture->operations, "buyer", &availability));
        CHECK(availability.booked == 50000 && availability.reserved == 12000 && availability.available == 38000);
        Cancel(fixture, "reserve", "buyer"); OK(UmiFinanceOperationsCash(fixture->operations, "buyer", &availability));
        CHECK(availability.reserved == 0); return;
    }
    if (strcmp(name, "cancel_owner") == 0) {
        Order(fixture, "resting", "seller", UMI_SIDE_SELL, 1000, 10);
        Begin(fixture, UMI_FINANCE_CANCEL_ORDER, "resting", "trader"); Id(&fixture->command.participantId, "buyer");
        Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    if (strcmp(name, "self_trade") == 0) {
        Inventory(fixture, "buyer", 5);
        Order(fixture, "other.sell", "seller", UMI_SIDE_SELL, 900, 1);
        Order(fixture, "self.sell", "buyer", UMI_SIDE_SELL, 1000, 1);
        OrderCommand(fixture, "self.buy", "buyer", UMI_SIDE_BUY, 1100, 2);
        Reject(fixture, UMI_STATUS_PERMISSION_DENIED);
        OK(UmiFinanceOperationsCounts(fixture->operations, &counts)); CHECK(counts.fills == 0U);
        OK(UmiFinanceOperationsOrderAt(fixture->operations, 0U, &order)); CHECK(order.remainingLots == 1); return;
    }
    if (strcmp(name, "price_time") == 0) {
        Order(fixture, "first", "seller", UMI_SIDE_SELL, 1000, 2);
        Order(fixture, "best", "seller", UMI_SIDE_SELL, 900, 2);
        Order(fixture, "second", "seller", UMI_SIDE_SELL, 1000, 2);
        Order(fixture, "take", "buyer", UMI_SIDE_BUY, 1100, 5);
        OK(UmiFinanceOperationsFillAt(fixture->operations, 0U, &fill)); CHECK(strcmp(fill.sellOrderId.value, "best") == 0);
        OK(UmiFinanceOperationsFillAt(fixture->operations, 1U, &fill)); CHECK(strcmp(fill.sellOrderId.value, "first") == 0);
        OK(UmiFinanceOperationsFillAt(fixture->operations, 2U, &fill)); CHECK(strcmp(fill.sellOrderId.value, "second") == 0 && fill.lots == 1);
        return;
    }
    if (strcmp(name, "resting_buy_price") == 0) {
        Order(fixture, "first.buy", "buyer", UMI_SIDE_BUY, 1100, 2);
        Order(fixture, "take.sell", "seller", UMI_SIDE_SELL, 900, 2);
        OK(UmiFinanceOperationsFillAt(fixture->operations, 0U, &fill)); CHECK(fill.executionTicks == 1100); return;
    }
    if (strcmp(name, "not_crossing") == 0) {
        Order(fixture, "first.sell", "seller", UMI_SIDE_SELL, 1100, 2);
        Order(fixture, "low.buy", "buyer", UMI_SIDE_BUY, 900, 2);
        CHECK(UmiFinanceOperationsFillAt(fixture->operations, 0U, &fill) == UMI_STATUS_NOT_FOUND); return;
    }
    if (strcmp(name, "journal_overflow") == 0) {
        Journal(fixture, "large", "control", "buyer.cash", INT64_MAX);
        Begin(fixture, UMI_FINANCE_APPROVE_JOURNAL, "large", "checker"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_POST_JOURNAL, "large", "checker"); Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED);
        CheckBalances(fixture, 50000, 0); return;
    }
    if (strcmp(name, "reversal") == 0) {
        Begin(fixture, UMI_FINANCE_REVERSE_JOURNAL, "reverse.funding", "maker");
        Id(&fixture->command.referenceId, "funding"); Id(&fixture->command.periodId, "september"); fixture->command.date = DAY; Apply(fixture);
        ApprovePost(fixture, "reverse.funding"); CheckBalances(fixture, 0, 0);
        OK(UmiFinanceOperationsJournalAt(fixture->operations, 0U, &journal));
        CHECK(journal.entry.status == UMI_ACCOUNTING_JOURNAL_REVERSED && strcmp(journal.reversedBy.value, "reverse.funding") == 0); return;
    }
    if (strcmp(name, "asof_report") == 0) {
        Period(fixture, "october", 10U);
        Begin(fixture, UMI_FINANCE_REVERSE_JOURNAL, "reverse.oct", "maker");
        Id(&fixture->command.referenceId, "funding"); Id(&fixture->command.periodId, "october");
        fixture->command.date = (UmiFinancialDate){2026, 10U, 1U}; Apply(fixture); ApprovePost(fixture, "reverse.oct");
        OK(UmiFinanceOperationsTrialBalance(fixture->operations, "september", GBP, 2U, &report));
        CHECK(report.closingDebitMinor == 50000 && report.balanced);
        OK(UmiFinanceOperationsTrialBalance(fixture->operations, "october", GBP, 2U, &report));
        CHECK(report.closingDebitMinor == 0 && report.periodDebitMinor == 50000 && report.balanced); return;
    }
    if (strcmp(name, "reversal_reserved") == 0) {
        Order(fixture, "promise", "buyer", UMI_SIDE_BUY, 1000, 1);
        Begin(fixture, UMI_FINANCE_REVERSE_JOURNAL, "reverse", "maker");
        Id(&fixture->command.referenceId, "funding"); Id(&fixture->command.periodId, "september"); fixture->command.date = DAY; Apply(fixture);
        Begin(fixture, UMI_FINANCE_APPROVE_JOURNAL, "reverse", "checker"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_POST_JOURNAL, "reverse", "checker"); Reject(fixture, UMI_STATUS_INVALID_STATE);
        CheckBalances(fixture, 50000, 0); return;
    }
    if (strcmp(name, "account_capacity") == 0) {
        for (size_t index = 3U; index < UMI_FINANCE_OPERATIONS_ACCOUNTS; ++index) {
            char id[48]; (void)snprintf(id, sizeof(id), "capacity.%zu", index);
            Account(fixture, id, UMI_ACCOUNTING_ASSET, GBP);
        }
        Begin(fixture, UMI_FINANCE_CREATE_ACCOUNT, "capacity.extra", "operator");
        strcpy(fixture->command.name, "Extra"); fixture->command.currency = GBP; fixture->command.scale = 2U;
        Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED); return;
    }
    if (strcmp(name, "order_capacity") == 0) {
        for (size_t index = 0U; index < UMI_FINANCE_OPERATIONS_ORDERS; ++index) {
            char id[48]; (void)snprintf(id, sizeof(id), "capacity.%zu", index);
            Order(fixture, id, "buyer", UMI_SIDE_BUY, 1, 1);
        }
        OrderCommand(fixture, "capacity.extra", "buyer", UMI_SIDE_BUY, 1, 1); Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED); return;
    }
    if (strcmp(name, "event_capacity") == 0) {
        while (Revision(fixture) < UMI_FINANCE_OPERATIONS_EVENTS) {
            Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator");
            fixture->command.enabled = (Revision(fixture) & 1U) == 0U; Apply(fixture);
        }
        Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiFinanceOperationsReload(fixture->operations)); CHECK(Revision(fixture) == UMI_FINANCE_OPERATIONS_EVENTS); return;
    }
    if (strcmp(name, "codec_truncation") == 0) {
        char encoded[FINANCE_RECORD_CAPACITY], truncated[FINANCE_RECORD_CAPACITY]; UmiFinanceOperationCommand decoded;
        OK(FinanceEncode(&fixture->command, encoded, sizeof(encoded)));
        for (size_t index = 0U; index < strlen(encoded); ++index) {
            memcpy(truncated, encoded, index); truncated[index] = '\0';
            CHECK(FinanceDecode(truncated, &decoded) == UMI_STATUS_PARSE_ERROR);
        }
        return;
    }
    if (strcmp(name, "utf8") == 0) {
        Begin(fixture, UMI_FINANCE_CREATE_ACCOUNT, "bad.name", "operator");
        fixture->command.name[0] = (char)0xc0; fixture->command.name[1] = (char)0x80;
        fixture->command.currency = GBP; fixture->command.scale = 2U; Reject(fixture, UMI_STATUS_INVALID_ARGUMENT);
        strcpy(fixture->command.name, "Caf\xc3\xa9 practice"); Apply(fixture); return;
    }
    if (strcmp(name, "reload_blocks_writes") == 0) {
        OK(umi_data_server_set(fixture->server, FINANCE_REVISION_KEY, "UFIN1:01"));
        CHECK(UmiFinanceOperationsReload(fixture->operations) == UMI_STATUS_PARSE_ERROR);
        Begin(fixture, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    Match(fixture);
    if (strcmp(name, "partial_fill") == 0) {
        OK(UmiFinanceOperationsFillAt(fixture->operations, 0U, &fill)); CHECK(fill.cash.minor_units == 6000 && fill.lots == 6);
        OK(UmiFinanceOperationsOrderAt(fixture->operations, 0U, &order)); CHECK(order.remainingLots == 4 && order.filledLots == 6);
        OK(UmiFinanceOperationsCash(fixture->operations, "buyer", &availability)); CHECK(availability.reserved == 6000 && availability.available == 44000);
        OK(UmiFinanceOperationsLots(fixture->operations, "seller", "practice", &availability)); CHECK(availability.reserved == 10);
        return;
    }
    if (strcmp(name, "clear_before_settle") == 0) {
        Begin(fixture, UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "settlement");
        Id(&fixture->command.periodId, "september"); fixture->command.date = DAY; Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "settlement_checker") == 0) {
        Begin(fixture, UMI_FINANCE_CLEAR_FILL, "system.fill.000001", "clearing"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "clearing");
        Id(&fixture->command.periodId, "september"); fixture->command.date = DAY; Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    if (strcmp(name, "settlement_date") == 0) {
        Begin(fixture, UMI_FINANCE_CLEAR_FILL, "system.fill.000001", "clearing"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "settlement");
        Id(&fixture->command.periodId, "september"); fixture->command.date = (UmiFinancialDate){2026,9U,24U};
        Reject(fixture, UMI_STATUS_INVALID_ARGUMENT); return;
    }
    if (strcmp(name, "close_unfinished") == 0) {
        ReconcileAll(fixture);
        Begin(fixture, UMI_FINANCE_PREPARE_CLOSE, "september", "closer"); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "settlement_inventory_overflow") == 0) {
        Inventory(fixture, "buyer", INT64_MAX);
        Begin(fixture, UMI_FINANCE_CLEAR_FILL, "system.fill.000001", "clearing"); Apply(fixture);
        Begin(fixture, UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "settlement");
        Id(&fixture->command.periodId, "september"); fixture->command.date = DAY;
        Reject(fixture, UMI_STATUS_CAPACITY_EXCEEDED); CheckBalances(fixture, 50000, 0); return;
    }
    Settle(fixture, "system.fill.000001");
    if (strcmp(name, "settlement") == 0) {
        CheckBalances(fixture, 44000, 6000);
        OK(UmiFinanceOperationsLots(fixture->operations, "buyer", "practice", &availability)); CHECK(availability.booked == 6);
        OK(UmiFinanceOperationsLots(fixture->operations, "seller", "practice", &availability)); CHECK(availability.booked == 94 && availability.reserved == 4);
        OK(UmiFinanceOperationsJournalAt(fixture->operations, 1U, &journal)); CHECK(journal.settlement && journal.entry.status == UMI_ACCOUNTING_JOURNAL_POSTED);
        return;
    }
    if (strcmp(name, "settlement_duplicate") == 0) {
        saved = fixture->command; before = Revision(fixture); Apply(fixture);
        CHECK(fixture->receipt.duplicate && Revision(fixture) == before); CheckBalances(fixture, 44000, 6000);
        Begin(fixture, UMI_FINANCE_SETTLE_FILL, saved.id.value, "settlement"); fixture->command.date = DAY;
        Id(&fixture->command.periodId, "september"); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "settlement_reversal_blocked") == 0) {
        Begin(fixture, UMI_FINANCE_REVERSE_JOURNAL, "bad.reverse", "maker");
        Id(&fixture->command.referenceId, "system.settlement.000001"); Id(&fixture->command.periodId, "september"); fixture->command.date = DAY;
        Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "replay") == 0) {
        before = Revision(fixture); OK(UmiFinanceOperationsReload(fixture->operations));
        CHECK(Revision(fixture) == before); CheckBalances(fixture, 44000, 6000);
        OK(UmiFinanceOperationsFillAt(fixture->operations, 0U, &fill)); CHECK(fill.state == UMI_SETTLEMENT_SETTLED); return;
    }
    if (strcmp(name, "corrupt_event") == 0 || strcmp(name, "orphan_event") == 0 || strcmp(name, "missing_event") == 0) {
        before = Revision(fixture);
        if (strcmp(name, "corrupt_event") == 0) OK(umi_data_server_set(fixture->server, FINANCE_EVENT_PREFIX "00000000000000000001", "broken"));
        else if (strcmp(name, "orphan_event") == 0) OK(umi_data_server_set(fixture->server, FINANCE_EVENT_PREFIX "00000000000000000500", "orphan"));
        else OK(umi_data_server_delete(fixture->server, FINANCE_EVENT_PREFIX "00000000000000000001"));
        CHECK(UmiFinanceOperationsReload(fixture->operations) == UMI_STATUS_PARSE_ERROR);
        CHECK(Revision(fixture) == before); CheckBalances(fixture, 44000, 6000); return;
    }
    Cancel(fixture, "sell.1", "seller");
    if (strcmp(name, "reconciliation_break") == 0) {
        ReconcileAll(fixture); Reconcile(fixture, "bad.recon", "buyer.cash", 1);
        Begin(fixture, UMI_FINANCE_PREPARE_CLOSE, "september", "closer"); Reject(fixture, UMI_STATUS_INVALID_STATE);
        CheckBalances(fixture, 44000, 6000); return;
    }
    ReconcileAll(fixture);
    if (strcmp(name, "reconciliation_stale") == 0) {
        Journal(fixture, "extra", "control", "buyer.cash", 1); ApprovePost(fixture, "extra");
        Begin(fixture, UMI_FINANCE_PREPARE_CLOSE, "september", "closer"); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    Begin(fixture, UMI_FINANCE_PREPARE_CLOSE, "september", "closer"); Apply(fixture);
    if (strcmp(name, "review_reopen") == 0) {
        Begin(fixture, UMI_FINANCE_REOPEN_REVIEW, "september", "closer"); Apply(fixture);
        OK(UmiFinanceOperationsPeriodAt(fixture->operations, 0U, &period)); CHECK(period.status == UMI_ACCOUNTING_PERIOD_OPEN); return;
    }
    if (strcmp(name, "close_checker") == 0) {
        Begin(fixture, UMI_FINANCE_CLOSE_PERIOD, "september", "closer"); Reject(fixture, UMI_STATUS_PERMISSION_DENIED); return;
    }
    Begin(fixture, UMI_FINANCE_CLOSE_PERIOD, "september", "reviewer"); Apply(fixture);
    if (strcmp(name, "close") == 0) {
        OK(UmiFinanceOperationsPeriodAt(fixture->operations, 0U, &period)); CHECK(period.status == UMI_ACCOUNTING_PERIOD_CLOSED); return;
    }
    if (strcmp(name, "closed_reopen") == 0) {
        Begin(fixture, UMI_FINANCE_REOPEN_REVIEW, "september", "closer"); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "closed_trading") == 0) {
        OrderCommand(fixture, "late", "buyer", UMI_SIDE_BUY, 1, 1); Reject(fixture, UMI_STATUS_INVALID_STATE); return;
    }
    if (strcmp(name, "audit") == 0) {
        UmiFinanceOperationAudit audit;
        before = Revision(fixture);
        for (size_t index = 0U; index < (size_t)before; ++index) {
            OK(UmiFinanceOperationsAuditAt(fixture->operations, index, &audit)); CHECK(audit.revision == (uint64_t)index + 1U);
            CHECK(audit.actorId.value[0] != '\0' && audit.requestId.value[0] != '\0');
        }
        OK(UmiFinanceOperationsReload(fixture->operations));
        OK(UmiFinanceOperationsAccountBalance(fixture->operations, "control", &value)); CHECK(value == 50000); return;
    }
    fprintf(stderr, "Unknown case: %s\n", name); exit(2);
}

static int SQLiteCase(const char *name, const char *path)
{
    Fixture a, b;
    uint64_t before;
    UmiDataServer *probe = NULL;
    UmiStatus probeStatus;
    (void)remove(path);
    probeStatus = umi_data_server_create_sqlite(path, &probe);
    if (probeStatus == UMI_STATUS_UNAVAILABLE) { puts("SQLite unavailable: skipped."); return 77; }
    CHECK(probeStatus == UMI_STATUS_OK); umi_data_server_destroy(probe);
    Create(&a, path); Setup(&a); Match(&a); Settle(&a, "system.fill.000001");
    if (strcmp(name, "restart") == 0) {
        UmiFinanceOperationCommand saved = a.command;
        before = Revision(&a); Destroy(&a); Create(&a, path);
        CHECK(Revision(&a) == before); CheckBalances(&a, 44000, 6000);
        OK(UmiFinanceOperationsApply(a.operations, &saved, &a.receipt)); CHECK(a.receipt.duplicate);
    } else if (strcmp(name, "stale_writer") == 0) {
        Create(&b, path); b.request = a.request + 1000U;
        Begin(&a, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); Apply(&a);
        Begin(&b, UMI_FINANCE_SET_MARKET_STATE, "practice", "different"); Reject(&b, UMI_STATUS_BUSY);
        OK(UmiFinanceOperationsReload(b.operations));
        b.command.expectedRevision = Revision(&b); Apply(&b); Destroy(&b);
    } else if (strcmp(name, "write_failure") == 0) {
        before = Revision(&a);
        OK(umi_data_server_execute(a.server, "PRAGMA query_only=ON;"));
        Begin(&a, UMI_FINANCE_SET_MARKET_STATE, "practice", "operator"); Reject(&a, UMI_STATUS_IO_ERROR);
        CHECK(!umi_data_server_in_transaction(a.server)); CHECK(Revision(&a) == before);
        OK(umi_data_server_execute(a.server, "PRAGMA query_only=OFF;")); Apply(&a);
    } else if (strcmp(name, "read_failure") == 0) {
        OK(umi_data_server_execute(a.server, "DROP TABLE umicom_kv;"));
        CHECK(UmiFinanceOperationsReload(a.operations) == UMI_STATUS_IO_ERROR);
    } else if (strcmp(name, "settlement_rollback") == 0) {
        /* Prepare a second fill, then deny the event write. Both money and
         * custody must remain unchanged, including after reopening the book. */
        Order(&a, "buy.more", "buyer", UMI_SIDE_BUY, 1000, 1);
        Begin(&a, UMI_FINANCE_CLEAR_FILL, "system.fill.000002", "clearing"); Apply(&a);
        OK(umi_data_server_execute(a.server, "PRAGMA query_only=ON;"));
        Begin(&a, UMI_FINANCE_SETTLE_FILL, "system.fill.000002", "settlement");
        Id(&a.command.periodId, "september"); a.command.date = DAY; Reject(&a, UMI_STATUS_IO_ERROR);
        CheckBalances(&a, 44000, 6000);
        OK(umi_data_server_execute(a.server, "PRAGMA query_only=OFF;")); Apply(&a);
        CheckBalances(&a, 43000, 7000);
    } else { fprintf(stderr, "Unknown SQLite case\n"); exit(2); }
    Destroy(&a); (void)remove(path); return 0;
}
int main(int argc, char **argv)
{
    Fixture fixture;
    if (argc < 2) return 2;
    if (strncmp(argv[1], "sqlite.", 7U) == 0) {
        if (argc != 3) return 2;
        return SQLiteCase(argv[1] + 7, argv[2]);
    }
    Create(&fixture, NULL); TestCase(argv[1], &fixture); Destroy(&fixture);
    printf("PASS %s\n", argv[1]); return 0;
}
