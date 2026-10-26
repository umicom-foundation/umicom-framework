/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/comparison_report.c
 *
 * PURPOSE:
 *   Export both imported observations with their comparison and page scope.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "comparison_private.h"

/* Export original observations, not calculated ledger adjustments. Repeating
 * scope on every row keeps filtering from hiding the input page limitations. */
UmiStatus UmiBankFeedComparisonExportCsv(const UmiBankFeedComparison *owner,
                                        UmiCsvDocument **out) {
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (owner == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *headers[] = {
        "record_kind", "comparison_index", "comparison", "changed_fields_mask",
        "before_index", "after_index", "side", "account_id", "transaction_id",
        "reference", "information", "booking_time", "currency", "amount",
        "direction", "status", "mutable", "before_previous_page", "before_next_page",
        "after_previous_page", "after_next_page", "scope"
    };
    UmiCsvCell cells[22];
    for (size_t i = 0U; i < 22U; ++i) cells[i] = UmiCsvText(headers[i]);
    UmiCsvDocument *csv = NULL;
    UmiStatus status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &csv);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(csv, cells, 22U);
    for (size_t i = 0U; i < 22U; ++i) cells[i] = UmiCsvText("");
    cells[0] = UmiCsvText("metadata");
    cells[17] = UmiCsvText(owner->summary.before.hasPreviousPage ? "true" : "false");
    cells[18] = UmiCsvText(owner->summary.before.hasNextPage ? "true" : "false");
    cells[19] = UmiCsvText(owner->summary.after.hasPreviousPage ? "true" : "false");
    cells[20] = UmiCsvText(owner->summary.after.hasNextPage ? "true" : "false");
    cells[21] = UmiCsvText("Imported observations; completeness and authenticity unverified; absence is not deletion");
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(csv, cells, 22U);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < owner->summary.rows; ++i) {
        const BankComparisonEntry *entry = &owner->entries[i];
        for (unsigned int side = 0U; status == UMI_STATUS_OK && side < 2U; ++side) {
            size_t index = side == 0U ? entry->before : entry->after;
            if (index == SIZE_MAX) continue;
            const UmiBankFeedRow *row = side == 0U ? &owner->before[index] : &owner->after[index];
            cells[0] = UmiCsvText("observation");
            cells[1] = UmiCsvUnsigned((uint64_t)i);
            cells[2] = UmiCsvText(UmiBankFeedComparisonKindName(entry->kind));
            cells[3] = UmiCsvUnsigned(entry->changes);
            cells[4] = entry->before == SIZE_MAX ? UmiCsvText("") : UmiCsvUnsigned((uint64_t)entry->before);
            cells[5] = entry->after == SIZE_MAX ? UmiCsvText("") : UmiCsvUnsigned((uint64_t)entry->after);
            cells[6] = UmiCsvText(side == 0U ? "before" : "after");
            cells[7] = UmiCsvText(row->account);
            cells[8] = UmiCsvText(row->transaction);
            cells[9] = UmiCsvText(row->reference);
            cells[10] = UmiCsvText(row->information);
            cells[11] = UmiCsvText(row->bookingTime);
            cells[12] = UmiCsvText(row->currency);
            cells[13] = UmiCsvText(row->amountText);
            cells[14] = UmiCsvText(row->credit ? "Credit" : "Debit");
            cells[15] = UmiCsvText(row->booked ? "Booked" : "Pending");
            cells[16] = UmiCsvText(row->mutableRecord ? "true" : "false");
            status = UmiCsvDocumentAppendRow(csv, cells, 22U);
        }
    }
    if (status == UMI_STATUS_OK)
        *out = csv;
    else
        UmiCsvDocumentDestroy(csv);
    return status;
}
