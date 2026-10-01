/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/test_contract.c
 * PURPOSE: Protect historical action values, exact event round trips and copied queries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"
_Static_assert(UMI_BANK_RECONCILE == 22 && UMI_BANK_INTEREST_SUBMIT == 23 && UMI_BANK_CHARGE_REVERSE == 34, "Historical actions changed");
_Static_assert(UMI_BANK_RECONCILIATION_RESOLVE == 35 && UMI_BANK_RECONCILIATION_REOPEN == 36, "Investigation action values changed");

/* Storage uses canonical command bytes, never the expanded public structure. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0};
    OK(UmiBankOperationsOpenMemory(&f.bank)); ReconciliationSetup(&f);
    if (strcmp(name, "codec") == 0) {
        UmiBankCommand c = Resolution(&f); ApplyResolution(&f, &c); c = Reopen(&f); ApplyResolution(&f, &c);
        for (size_t i = 0U; i < 7U; ++i) {
            UmiBankAuditEvent event, decoded; char first[BANK_RECORD_TEXT_CAPACITY], second[BANK_RECORD_TEXT_CAPACITY];
            OK(UmiBankOperationsAuditAt(f.bank, i, &event)); OK(BankEncode(&event, first, sizeof first));
            OK(BankDecode(first, &decoded)); OK(BankEncode(&decoded, second, sizeof second));
            CHECK(strcmp(first, second) == 0 && decoded.command.action == event.command.action &&
                strcmp(decoded.command.name, event.command.name) == 0 && strcmp(decoded.command.ownerId.value, event.command.ownerId.value) == 0);
        }
    } else if (strcmp(name, "fields") == 0) {
        CHECK(UmiBankActionFields(UMI_BANK_RECONCILIATION_RESOLVE) == (UMI_BANK_FIELD_OWNER | UMI_BANK_FIELD_NAME));
        CHECK(UmiBankActionFields(UMI_BANK_RECONCILIATION_REOPEN) == UMI_BANK_FIELD_NAME);
        CHECK(strcmp(UmiBankActionName(UMI_BANK_RECONCILIATION_RESOLVE), "Resolve reconciliation break") == 0);
        CHECK(strcmp(UmiBankActionName(UMI_BANK_RECONCILIATION_REOPEN), "Reopen reconciliation break") == 0);
    } else if (strcmp(name, "query") == 0) {
        UmiBankReconciliation record = ReadBreak(&f); record.externalBalance.minor_units = 0;
        strcpy(record.reviewReason, "Local copy"); CHECK(ReadBreak(&f).externalBalance.minor_units == 90000 && ReadBreak(&f).reviewReason[0] == '\0');
        CHECK(UmiBankOperationsFindReconciliation(f.bank, "missing", &record) == UMI_STATUS_NOT_FOUND && record.id.value[0] == '\0');
        CHECK(UmiBankOperationsFindReconciliation(NULL, "break", &record) == UMI_STATUS_INVALID_ARGUMENT && record.id.value[0] == '\0');
        char tooLong[UMI_FINANCE_ID_CAPACITY + 1U]; memset(tooLong, 'x', sizeof tooLong); tooLong[sizeof tooLong - 1U] = '\0';
        CHECK(UmiBankOperationsFindReconciliation(f.bank, tooLong, &record) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "labels") == 0) {
        UmiBankReconciliation record = ReadBreak(&f); CHECK(strcmp(UmiBankReconciliationStateName(&record), "Open break") == 0);
        record.disposition = UMI_BANK_RECONCILIATION_RESOLVED; CHECK(strcmp(UmiBankReconciliationStateName(&record), "Resolved break") == 0);
        record.disposition = UMI_BANK_RECONCILIATION_REOPENED; CHECK(strcmp(UmiBankReconciliationStateName(&record), "Reopened break") == 0);
        record.matched = true; record.disposition = UMI_BANK_RECONCILIATION_UNREVIEWED; CHECK(strcmp(UmiBankReconciliationStateName(&record), "Matched") == 0);
        CHECK(strcmp(UmiBankReconciliationStateName(NULL), "Unknown") == 0);
    } else return 2;
    UmiBankOperationsDestroy(f.bank); return 0;
}
