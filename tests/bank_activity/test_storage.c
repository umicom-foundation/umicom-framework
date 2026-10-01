/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/test_storage.c
 * PURPOSE: Keep captured reports stable across database reopen and explicit reload from a second writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[1]; Fixture f = {0};
    FILE *file = fopen(argv[2], "wx"); CHECK(file != NULL && fclose(file) == 0);
    UmiStatus status = UmiBankOperationsOpenSqlite(argv[2], &f.bank);
    if (status == UMI_STATUS_UNAVAILABLE) { CHECK(remove(argv[2]) == 0); return 77; }
    OK(status); ActivitySetup(&f); UmiBankActivityQuery query = ActivityQuery();
    UmiBankActivity *old = ActivityCapture(&f, query); UmiBankActivitySummary summary;
    OK(UmiBankActivityReadSummary(old, &summary)); CHECK(summary.durable && summary.count == 4 && summary.revision == 9);
    if (strcmp(name, "reopen") == 0) {
        UmiBankOperationsDestroy(f.bank); f.bank = NULL; OK(UmiBankOperationsOpenSqlite(argv[2], &f.bank));
        UmiBankActivity *current = ActivityCapture(&f, query); UmiBankActivitySummary other; OK(UmiBankActivityReadSummary(current, &other));
        CHECK(other.count == 4 && other.balance.booked.minor_units == 100500 && other.balance.reserved.minor_units == 1000);
        UmiBankActivityRow row; OK(UmiBankActivityRowAt(current, 3, &row)); CHECK(row.reversal && row.posting.businessDate.day == 29);
        UmiBankActivityDestroy(current);
    } else if (strcmp(name, "cached-reload") == 0) {
        Fixture writer = {0}; writer.serial = 100; writer.operator = f.operator;
        OK(UmiBankOperationsOpenSqlite(argv[2], &writer.bank));
        UmiBankCommand c = Make(&writer, UMI_BANK_TEST_CREDIT, "new-commit"); Id(&c.sourceAccountId, "account"); c.amount = Cash(500); Send(&writer, &writer.operator, &c);
        UmiBankActivity *cached = ActivityCapture(&f, query); OK(UmiBankActivityReadSummary(cached, &summary)); CHECK(summary.revision == 9 && summary.count == 4);
        UmiBankActivityDestroy(cached); OK(UmiBankOperationsReload(f.bank));
        UmiBankActivity *current = ActivityCapture(&f, query); OK(UmiBankActivityReadSummary(current, &summary));
        CHECK(summary.revision == 10 && summary.count == 5 && summary.balance.booked.minor_units == 101000);
        OK(UmiBankActivityReadSummary(old, &summary)); CHECK(summary.revision == 9 && summary.balance.booked.minor_units == 100500);
        UmiBankActivityDestroy(current); UmiBankOperationsDestroy(writer.bank);
    } else return 2;
    UmiBankActivityDestroy(old); UmiBankOperationsDestroy(f.bank); CHECK(remove(argv[2]) == 0); return 0;
}
