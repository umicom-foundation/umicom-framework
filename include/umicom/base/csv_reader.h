/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/csv_reader.h
 * PURPOSE: Read bounded CSV records without interpreting their values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_CSV_READER_H
#define UMICOM_BASE_CSV_READER_H
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCsvReader UmiCsvReader;
typedef struct UmiCsvReadRow {
    const char *fields[UMI_CSV_MAX_COLUMNS];
    size_t lengths[UMI_CSV_MAX_COLUMNS];
    size_t count, record;
} UmiCsvReadRow;
/** Copy at most UMI_CSV_MAX_BYTES of UTF-8 input into an independent reader.
 * Comma delimiters, doubled quotes, quoted multiline fields, LF and CRLF are
 * supported. An initial UTF-8 BOM is skipped. Bare CR, NUL, unsupported controls
 * and malformed UTF-8 are rejected. NULL is accepted only for zero bytes.
 * Values are text; no numeric conversion, whitespace trimming or formula
 * execution occurs. Failure clears *out. Use each reader on one owning thread. */
UmiStatus UmiCsvReaderCreate(const void *bytes, size_t length, UmiCsvReader **out);
/** Decode the next record, with at most 64 fields of at most 65536 bytes each.
 * Fields are NUL-terminated and borrowed until the next Next call or Destroy,
 * including a Next call that fails. Copy them before retaining them elsewhere.
 * record is one-based and counts logical records, not physical text lines.
 * Empty fields and blank records are retained; a trailing newline adds no record.
 * NOT_FOUND means end of input. Parse/capacity errors are terminal and repeat on later
 * calls. Failure leaves out unchanged. No partly parsed record is returned. */
UmiStatus UmiCsvReaderNext(UmiCsvReader *reader, UmiCsvReadRow *out);
/** Release the source copy and borrowed field storage. NULL is accepted. */
void UmiCsvReaderDestroy(UmiCsvReader *reader);
#ifdef __cplusplus
}
#endif
#endif
