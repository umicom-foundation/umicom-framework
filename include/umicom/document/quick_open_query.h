/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/quick_open_query.h
 * PURPOSE: Separate a filename filter from an optional source position.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_QUICK_OPEN_QUERY_H
#define UMICOM_DOCUMENT_QUICK_OPEN_QUERY_H
#include "umicom/document/navigation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_QUICK_OPEN_QUERY_MAX 1024U
typedef struct UmiQuickOpenQuery {
    char filter[UMI_QUICK_OPEN_QUERY_MAX + 1U];
    UmiDocumentLocation location;
    int has_location;
} UmiQuickOpenQuery;
/** Parse a bounded filename filter with optional :line or :line:column.
 * Lines and UTF-8 byte columns are one-based. Numeric parsing follows
 * UmiDocumentLocationParse, including overflow refusal. No file is opened.
 * The first colon after the final path separator starts an unquoted suffix;
 * a Windows drive prefix is not a suffix. For literal colons, quote the whole
 * filename filter: "notes:12" or "notes:12":4. Quotes are not escaped.
 * An empty unquoted query lists all files, but a location requires a nonempty
 * filter. A quoted filter must be nonempty. Spaces in the filter are retained.
 * Input is exactly byteCount readable bytes; embedded NUL is refused. The
 * caller supplies UTF-8 when presenting the result through a Unicode toolkit.
 * No allocation or I/O occurs. Errors preserve outQuery. Oversized input is
 * CAPACITY_EXCEEDED; syntax, empty positions and null arguments are INVALID_ARGUMENT. */
UmiStatus UmiQuickOpenQueryParse(const char *text, size_t byteCount, UmiQuickOpenQuery *outQuery);
#ifdef __cplusplus
}
#endif
#endif
