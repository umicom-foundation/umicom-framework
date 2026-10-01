/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/activity_csv.c
 * PURPOSE: Export exact captured activity filters, complete balances and matching posting evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "activity_private.h"
UmiStatus UmiBankActivityExportCsv(const UmiBankActivity *activity, UmiCsvDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (activity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *document = NULL;
    UmiStatus status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    if (status != UMI_STATUS_OK) return status;
    static const char *const names[] = {"record","scope","account_id","currency","scale","captured_revision","storage",
        "from_date_inclusive","to_date_inclusive","direction","reference_contains","matched_postings","total_postings",
        "complete_booked_minor","complete_reserved_minor","complete_available_minor",
        "matching_debit_minor","matching_credit_minor","matching_net_minor",
        "posting_revision","business_date","journal_id","reference_id","actor_id","action","reversal",
        "posting_debit_minor","posting_credit_minor","recorded_booked_balance_minor"};
    UmiCsvCell cells[sizeof names / sizeof names[0]]; size_t count = sizeof cells / sizeof cells[0];
    for (size_t i = 0; i < count; ++i) cells[i] = UmiCsvText(names[i]);
    status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0; i < count; ++i) cells[i] = UmiCsvText("");
    const UmiBankActivitySummary *s = &activity->summary;
    char from[11], to[11]; BankActivityDateText(s->query.fromDate, from); BankActivityDateText(s->query.toDate, to);
    cells[0] = UmiCsvText("activity-summary"); cells[1] = UmiCsvText("LOCAL PRACTICE");
    cells[2] = UmiCsvText(s->query.accountId.value); cells[3] = UmiCsvText(s->balance.booked.currency.code);
    cells[4] = UmiCsvUnsigned(s->balance.booked.scale); cells[5] = UmiCsvUnsigned(s->revision);
    cells[6] = UmiCsvText(s->durable ? "local durable store" : "memory only");
    cells[7] = UmiCsvText(from); cells[8] = UmiCsvText(to); cells[9] = UmiCsvText(UmiBankActivityDirectionName(s->query.direction));
    cells[10] = UmiCsvText(s->query.reference); cells[11] = UmiCsvUnsigned(s->count); cells[12] = UmiCsvUnsigned(s->totalPostings);
    cells[13] = UmiCsvSigned(s->balance.booked.minor_units); cells[14] = UmiCsvSigned(s->balance.reserved.minor_units);
    cells[15] = UmiCsvSigned(s->balance.available.minor_units); cells[16] = UmiCsvSigned(s->debitMinor);
    cells[17] = UmiCsvSigned(s->creditMinor); cells[18] = UmiCsvSigned(s->netMinor);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, cells, count);
    for (size_t i = 0; status == UMI_STATUS_OK && i < s->count; ++i) {
        const UmiBankActivityRow *row = &activity->rows[i]; const UmiBankStatementLine *line = &row->posting;
        char date[11]; BankActivityDateText(line->businessDate, date);
        /* Context remains on every row; totals occur only on the summary row
         * so spreadsheet users do not accidentally sum repeated balances. */
        for (size_t j = 11; j < count; ++j) cells[j] = UmiCsvText("");
        cells[0] = UmiCsvText("posting"); cells[19] = UmiCsvUnsigned(line->revision); cells[20] = UmiCsvText(date);
        cells[21] = UmiCsvText(line->journalId.value); cells[22] = UmiCsvText(line->referenceId.value);
        cells[23] = UmiCsvText(row->actorId.value); cells[24] = UmiCsvText(UmiBankActionName(row->action));
        cells[25] = UmiCsvText(row->reversal ? "yes" : "no");
        cells[26] = UmiCsvSigned(line->debitMinor); cells[27] = UmiCsvSigned(line->creditMinor);
        cells[28] = UmiCsvSigned(line->balanceMinor);
        status = UmiCsvDocumentAppendRow(document, cells, count);
    }
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(document); return status; }
    *outDocument = document; return UMI_STATUS_OK;
}
