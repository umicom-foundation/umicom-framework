/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/base64.h
 *
 * PURPOSE:
 *   Validate and decode canonical base64 without allocation or partial output writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BASE_BASE64_H
#define UMICOM_BASE_BASE64_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Default is the padded RFC 4648 alphabet. This optional flag permits only
 * ASCII space, tab, CR, LF, form feed and vertical tab between symbols. It does
 * not permit ignored punctuation, URL-safe symbols or incomplete quartets. */
#define UMI_BASE64_IGNORE_ASCII_WHITESPACE 1U
/** Validate exactly length bytes and report decoded size. Empty input is valid;
 * NULL text is accepted only for zero length. Reject embedded NUL, misplaced
 * padding, nonzero pad bits and content after a padded quartet. Unknown flags
 * are invalid arguments. out_count is required and unchanged on failure.
 * The input span must be readable and must not overlap out_count. */
UmiStatus UmiBase64DecodedSize(const char *text, size_t length,
    unsigned flags, size_t *out_count);
/** Decode only after the entire input and output capacity have been admitted.
 * Input must remain unchanged for the duration of both passes. No allocation,
 * filesystem or network access occurs. Refusal leaves output bytes and count
 * unchanged. NULL bytes is accepted only for zero decoded bytes. capacity is
 * the writable byte extent, with no terminator appended. Input, used output
 * and out_count may not overlap. Invalid spans/overlap return INVALID_ARGUMENT,
 * invalid encoding PARSE_ERROR, and insufficient space CAPACITY_EXCEEDED. */
UmiStatus UmiBase64Decode(const char *text, size_t length, unsigned flags,
    unsigned char *bytes, size_t capacity, size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
