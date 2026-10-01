/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/test_reports.c
 * PURPOSE: Compare textual and CSV evidence with independently expected monetary values and capture identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; OK(UmiBankOperationsOpenMemory(&f.bank)); ActivitySetup(&f);
    UmiBankActivityQuery query = ActivityQuery(); query.fromDate = (UmiFinancialDate){2026,10,1}; query.toDate = (UmiFinancialDate){2026,10,2};
    if (strcmp(name, "empty") == 0) strcpy(query.reference, "missing");
    UmiBankActivity *report = ActivityCapture(&f, query); size_t required = 0;
    OK(UmiBankActivityDescribe(report, NULL, 0, &required)); CHECK(required > 100);
    char *text = malloc(required); CHECK(text != NULL); OK(UmiBankActivityDescribe(report, text, required, NULL));
    if (strcmp(name, "text") == 0) {
        CHECK(strstr(text, "Business dates (inclusive): 2026-10-01 to 2026-10-02") != NULL);
        CHECK(strstr(text, "Booked: GBP 1005.00") != NULL);
        CHECK(strstr(text, "Net credits minus debits: GBP -20.00") != NULL);
        CHECK(strstr(text, "These totals are not opening, closing or available balances.") != NULL);
        CHECK(strstr(text, "Recorded booked balance after this posting (all prior postings)") != NULL);
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(strlen(text)+1 == required); size_t needed = 0;
        CHECK(UmiBankActivityDescribe(report, text, required-1, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(text[0] == '\0' && needed == required);
        CHECK(UmiBankActivityDescribe(NULL, text, required, &needed) == UMI_STATUS_INVALID_ARGUMENT && needed == 0 && text[0] == '\0');
    } else if (strcmp(name, "csv") == 0 || strcmp(name, "empty") == 0 || strcmp(name, "immutable") == 0) {
        if (strcmp(name, "immutable") == 0) {
            UmiBankCommand c = Make(&f, UMI_BANK_TEST_CREDIT, "later"); Id(&c.sourceAccountId, "account"); c.amount = Cash(500); Send(&f, &f.operator, &c);
            UmiBankOperationsDestroy(f.bank); f.bank = NULL;
        }
        UmiCsvDocument *csv = NULL; OK(UmiBankActivityExportCsv(report, &csv));
        const char *data = UmiCsvDocumentData(csv);
        CHECK(strstr(data, "\"LOCAL PRACTICE\"") != NULL && strstr(data, "\"captured_revision\"") != NULL);
        CHECK(strstr(data, "\"complete_booked_minor\"") != NULL && strstr(data, "\"100500\"") != NULL);
        CHECK(strstr(data, "\"1000\"") != NULL && strstr(data, "\"99500\"") != NULL);
        if (strcmp(name, "empty") == 0) {
            CHECK(UmiCsvDocumentRows(csv) == 2 && strstr(text, "No postings match") != NULL);
        } else {
            CHECK(UmiCsvDocumentRows(csv) == 4 && strstr(data, "\"-2000\"") != NULL && strstr(data, "\"98000\"") != NULL);
            CHECK(strstr(data, "\"later\"") == NULL && strstr(data, "\"2026-10-02\"") != NULL);
        }
        UmiCsvDocumentDestroy(csv);
    } else return 2;
    free(text); UmiBankActivityDestroy(report); UmiBankOperationsDestroy(f.bank); return 0;
}
