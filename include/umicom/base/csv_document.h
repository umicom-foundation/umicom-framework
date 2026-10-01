/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/csv_document.h
 * PURPOSE: Build bounded, owned CSV reports with atomic rows and typed numbers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_CSV_DOCUMENT_H
#define UMICOM_BASE_CSV_DOCUMENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CSV_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_CSV_MAX_COLUMNS 64U
#define UMI_CSV_MAX_TEXT_BYTES 65536U
typedef struct UmiCsvDocument UmiCsvDocument;
typedef enum UmiCsvKind {
    UMI_CSV_TEXT, UMI_CSV_SIGNED, UMI_CSV_UNSIGNED, UMI_CSV_REAL
} UmiCsvKind;
typedef struct UmiCsvCell {
    UmiCsvKind kind;
    const char *text;
    size_t length;
    int64_t signedValue;
    uint64_t unsignedValue;
    double realValue;
} UmiCsvCell;
/* Text spans need not be terminated. NULL is allowed only for an empty span.
 * The convenience Text constructor requires a terminated string. Constructors
 * do not validate or retain their input; AppendRow makes the owned copy. */
UmiCsvCell UmiCsvTextSpan(const char *text, size_t length);
UmiCsvCell UmiCsvText(const char *text);
UmiCsvCell UmiCsvSigned(int64_t value);
UmiCsvCell UmiCsvUnsigned(uint64_t value);
UmiCsvCell UmiCsvReal(double value);
/* maxBytes includes the terminating NUL, must be 1..UMI_CSV_MAX_BYTES.
 * On failure *outDocument is NULL. Destroy accepts NULL. */
UmiStatus UmiCsvDocumentCreate(size_t maxBytes, UmiCsvDocument **outDocument);
void UmiCsvDocumentDestroy(UmiCsvDocument *document);
/* Append 1..64 cells as one row. The first row fixes the column count. All
 * failures preserve bytes, row count and column count. Input may reference
 * the existing document. Text must be valid UTF-8 without NUL or control
 * characters except TAB/CR/LF. Quotes double, every cell is quoted, and rows
 * end in CRLF. Potential formula text gets a leading apostrophe: =,+,-,@
 * after leading ASCII whitespace/BOM, or an initial TAB/CR/LF. Typed numbers
 * are exempt; real numbers must be finite. This is a text export policy,
 * not a guarantee about every spreadsheet's import settings.
 * Real values retain DBL_DECIMAL_DIG precision and a dot decimal separator.
 * Use on one owning thread; do not change the C locale concurrently. */
UmiStatus UmiCsvDocumentAppendRow(UmiCsvDocument *document,
    const UmiCsvCell *cells, size_t count);
/* Borrowed UTF-8 remains valid until successful append or destruction.
 * Bytes excludes NUL. NULL document yields NULL data and zero counts. */
const char *UmiCsvDocumentData(const UmiCsvDocument *document);
size_t UmiCsvDocumentBytes(const UmiCsvDocument *document);
size_t UmiCsvDocumentRows(const UmiCsvDocument *document);
/* Includes NUL in requiredCapacity. NULL output/zero capacity is a size query.
 * Other failures leave caller output unchanged. Overlapping copies are valid. */
UmiStatus UmiCsvDocumentCopy(const UmiCsvDocument *document, char *output,
    size_t capacity, size_t *requiredCapacity);
#ifdef __cplusplus
}
#endif
#endif
