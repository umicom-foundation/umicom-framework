# Building an owned CSV report

CSV is a text format for tables. A comma separates cells; quotation marks keep
commas, line breaks and quotation marks inside a cell. Framework provides
`UmiCsvDocument` so applications can share one set of escaping, ownership and
size rules.

## Create a small report

1. Include `umicom/base/csv_document.h` and link the existing `Umicom::base`
   library.
2. Create a document with a maximum byte size, including its final zero byte.
3. Append the header and each data row. Every row must have the same number of
   cells. Check the returned status each time.
4. Read the complete UTF-8 bytes with `UmiCsvDocumentData` and
   `UmiCsvDocumentBytes`. The borrowed pointer stays valid until a successful
   append or destruction. `UmiCsvDocumentCopy` makes a caller-owned copy.
5. Destroy the document when finished. Text supplied to an append is copied;
   callers can release their original values as soon as the call returns.

```c
#include "umicom/base/csv_document.h"
#include <stdio.h>

int main(void)
{
    UmiCsvDocument *report = NULL;
    UmiStatus status = UmiCsvDocumentCreate(4096, &report);
    UmiCsvCell heading[] = {UmiCsvText("item"), UmiCsvText("count")};
    UmiCsvCell row[] = {UmiCsvText("Learning notes, September"), UmiCsvUnsigned(3)};
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(report, heading, 2);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(report, row, 2);
    if (status == UMI_STATUS_OK) {
        size_t bytes = UmiCsvDocumentBytes(report);
        if (fwrite(UmiCsvDocumentData(report), 1, bytes, stdout) != bytes)
            status = UMI_STATUS_IO_ERROR;
    }
    UmiCsvDocumentDestroy(report);
    return status == UMI_STATUS_OK ? 0 : 1;
}
```

The second row contains two cells even though the item contains a comma. A
failed append leaves the previous complete report unchanged. It does not
publish a partial row or silently omit a long value.

## Choose a cell type

Use `UmiCsvText` for terminated text and `UmiCsvTextSpan` for a byte span. Both
require valid UTF-8. A zero byte inside a span, malformed UTF-8 and control
characters other than tab, carriage return and line feed are rejected.

Use `UmiCsvSigned` or `UmiCsvUnsigned` for exact integer values. Use `UmiCsvReal`
for an existing floating-point value: it writes enough significant digits for
a double, uses a dot for the decimal point, and rejects infinity and NaN.
Scientific notation may appear. This does not turn a floating-point amount
into exact financial arithmetic; Bank therefore exports integer minor units.

Text beginning with a potential spreadsheet formula receives a leading
apostrophe. This includes `=`, `+`, `-` or `@` after leading ASCII spaces,
tabs, line breaks or a UTF-8 byte-order mark. Leading tabs and line breaks also
receive the prefix. A text value `-12` becomes `'-12`; a signed numeric value
of negative twelve remains `-12`. The apostrophe is part of the exported text
and may be visible in another CSV reader. The original model is unchanged.
Spreadsheet import settings still matter; this policy cannot control every
spreadsheet's interpretation of the file.

## Use the domain reports

| Report | Function | What the metadata explains |
| --- | --- | --- |
| Trader orders | `UmiTradingWorkspaceExportOrdersCsv` | Applied search and status, order coordinator revision, total retained and matching orders |
| Bank statement | `UmiBankOperationsExportStatementCsv` | Local-practice scope, account, currency, scale, revision range, balances and storage mode |
| Studio evidence | `UmiTestEvidenceExportCsv` | Exact selected item/session filter, registry revisions and retained/matching result and output counts |

Each function returns a complete owned document or sets its output pointer to
NULL on failure. An empty result still has a header and summary row. Keep owner
access on its owning thread; these functions do not lock a concurrently changed
workspace. Reports remain readable after the original model is changed or
destroyed. Capturing one does not execute orders, post ledger entries, run
tests, reload storage or write files.

## Understand limits and import errors

A document holds at most 4 MiB including the terminator, a row has at most 64
cells, and one text cell has at most 65,536 bytes. Capacity errors return a
status rather than truncating evidence. Existing enterprise-grid and analytics
CSV helpers remain available with their established contracts.

For a spreadsheet, import as UTF-8 with comma separators and quoted fields.
Import identifiers and very large integers as text to preserve leading zeroes
and all digits; many spreadsheets round long numeric values. A date such as
`2026-09-30` may also be imported as text to keep its exact representation. Do
not change the C locale concurrently with an export. Diagnostic output that
contains terminal escape controls must be reviewed in its original output view
when a CSV export rejects it.
