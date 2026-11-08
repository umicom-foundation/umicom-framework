/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/utf8.h
 *
 * PURPOSE:
 *   Validate Unicode scalar sequences without document, UI or toolchain dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_UTF8_H
#define UMICOM_BASE_UTF8_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Decode one scalar from a bounded byte span. Reject overlong encodings,
 * surrogate code points, truncated sequences and values above U+10FFFF.
 * Embedded NUL is a valid scalar. Outputs remain unchanged on failure.
 * The caller supplies readable bytes and separate writable output objects. */
int UmiUtf8DecodeScalar(const unsigned char *bytes, size_t count,
    size_t *out_used, uint32_t *out_scalar);
/* Validate the whole span. A null pointer is allowed only for an empty span.
 * On a malformed sequence, report its starting byte offset; on success report
 * byte_count. For a null nonempty span, leave the optional offset unchanged.
 * This validates Unicode encoding, not display safety or normalization. */
int UmiUtf8Validate(const unsigned char *bytes, size_t byte_count,
    size_t *out_error_offset);
#ifdef __cplusplus
}
#endif
#endif
