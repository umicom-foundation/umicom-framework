/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_statement_csv.c
 * PURPOSE: Verify exact statement units, revision scope and immutable report ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../bank_interest/fixture.h"
#include "umicom/bank_operations/statement_csv.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); Fixture f = {0}; OK(UmiBankOperationsOpenMemory(&f.bank)); Setup(&f);
    UmiBankCommand c = Interest(&f, "interest", "2026-09"); Send(&f, &f.maker, &c);
    Step(&f, UMI_BANK_INTEREST_APPROVE, &f.checker); Step(&f, UMI_BANK_INTEREST_POST, &f.operator);
    UmiCsvDocument *doc = NULL; UmiBankCounts before, after;
    OK(UmiBankOperationsCounts(f.bank, &before));
    if (strcmp(argv[1], "empty") == 0) {
        OK(UmiBankOperationsExportStatementCsv(f.bank, "account", 4, 5, &doc));
        CHECK(UmiCsvDocumentRows(doc) == 2);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"100000\",\"100000\"") != NULL);
    } else if (strcmp(argv[1], "invalid") == 0) {
        CHECK(UmiBankOperationsExportStatementCsv(f.bank, "account", 6, 5, &doc) == UMI_STATUS_INVALID_ARGUMENT && doc == NULL);
        CHECK(UmiBankOperationsExportStatementCsv(f.bank, "missing", 1, 6, &doc) == UMI_STATUS_NOT_FOUND && doc == NULL);
        CHECK(UmiBankOperationsExportStatementCsv(f.bank, "account", 1, 7, &doc) == UMI_STATUS_INVALID_ARGUMENT && doc == NULL);
        CHECK(UmiBankOperationsExportStatementCsv(f.bank, "account", 1, 6, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "reversal") == 0) {
        Step(&f, UMI_BANK_INTEREST_REVERSE, &f.operator); OK(UmiBankOperationsCounts(f.bank, &before));
        OK(UmiBankOperationsExportStatementCsv(f.bank, "account", 6, 7, &doc));
        CHECK(UmiCsvDocumentRows(doc) == 4);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"410\",\"0\",\"100000\"") != NULL);
    } else {
        CHECK(strcmp(argv[1], "range") == 0 || strcmp(argv[1], "ownership") == 0);
        OK(UmiBankOperationsExportStatementCsv(f.bank, "account", 6, 6, &doc));
        CHECK(UmiCsvDocumentRows(doc) == 3);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"LOCAL PRACTICE\"") != NULL);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"GBP\",\"2\"") != NULL);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"100000\",\"100410\"") != NULL);
        CHECK(strstr(UmiCsvDocumentData(doc), "\"2026-09-30\",\"0\",\"410\",\"100410\"") != NULL);
        CHECK(strstr(UmiCsvDocumentData(doc), "funding") == NULL);
    }
    OK(UmiBankOperationsCounts(f.bank, &after)); CHECK(before.revision == after.revision && before.journals == after.journals);
    UmiBankOperationsDestroy(f.bank);
    if (strcmp(argv[1], "ownership") == 0) CHECK(strstr(UmiCsvDocumentData(doc), "\"100410\"") != NULL);
    UmiCsvDocumentDestroy(doc); return 0;
}
