/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/statement_csv.c
 * PURPOSE: Retain exact minor-unit statement evidence in an owned CSV report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_operations/statement_csv.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

UmiStatus UmiBankOperationsExportStatementCsv(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    UmiCsvDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    UmiBankStatement *statement = calloc(1U, sizeof(*statement));
    if (statement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiBankCounts counts;
    UmiStatus status = UmiBankOperationsCounts(operations, &counts);
    if (status == UMI_STATUS_OK) status = UmiBankOperationsStatement(operations,
        accountId, firstRevision, lastRevision, statement);
    UmiCsvDocument *document = NULL;
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    if (status != UMI_STATUS_OK) { free(statement); return status; }
    static const char *const names[] = {"record", "scope", "account_id", "currency", "scale",
        "first_revision", "last_revision", "captured_revision", "storage", "entry_count",
        "opening_minor", "closing_minor", "revision", "journal_id", "reference_id",
        "business_date", "debit_minor", "credit_minor", "balance_minor"};
    UmiCsvCell cells[sizeof(names) / sizeof(names[0])];
    const size_t count = sizeof(cells) / sizeof(cells[0]);
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText(names[i]);
    status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0U; i < count; ++i) cells[i] = UmiCsvText("");
    cells[0] = UmiCsvText("statement-summary");
    cells[1] = UmiCsvText("LOCAL PRACTICE");
    cells[2] = UmiCsvText(statement->accountId.value);
    cells[3] = UmiCsvText(statement->closing.currency.code);
    cells[4] = UmiCsvUnsigned(statement->closing.scale);
    cells[5] = UmiCsvUnsigned(statement->firstRevision);
    cells[6] = UmiCsvUnsigned(statement->lastRevision);
    cells[7] = UmiCsvUnsigned(counts.revision);
    cells[8] = UmiCsvText(counts.durable ? "local durable store" : "memory only");
    cells[9] = UmiCsvUnsigned(statement->count);
    cells[10] = UmiCsvSigned(statement->opening.minor_units);
    cells[11] = UmiCsvSigned(statement->closing.minor_units);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < statement->count; ++i) {
        const UmiBankStatementLine *line = &statement->lines[i];
        cells[0] = UmiCsvText("entry");
        cells[12] = UmiCsvUnsigned(line->revision);
        cells[13] = UmiCsvText(line->journalId.value);
        cells[14] = UmiCsvText(line->referenceId.value);
        char date[32];
        int written = snprintf(date, sizeof(date), "%04" PRId32 "-%02u-%02u",
            line->businessDate.year, (unsigned)line->businessDate.month, (unsigned)line->businessDate.day);
        if (written < 0 || (size_t)written >= sizeof(date)) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        cells[15] = UmiCsvText(date);
        cells[16] = UmiCsvSigned(line->debitMinor);
        cells[17] = UmiCsvSigned(line->creditMinor);
        cells[18] = UmiCsvSigned(line->balanceMinor);
        status = UmiCsvDocumentAppendRow(document, cells, count);
    }
    free(statement);
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(document); return status; }
    *outDocument = document;
    return UMI_STATUS_OK;
}
