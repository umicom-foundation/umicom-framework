/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/ascii_number.h
 * PURPOSE: Convert bounded decimal text without changing the process locale.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_ASCII_NUMBER_H
#define UMICOM_BASE_ASCII_NUMBER_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_ASCII_NUMBER_MAX_BYTES 511U
/* Read exactly length bytes; embedded NUL, whitespace, hex and trailing text
 * are refused. Integer spelling is [+-]digits; unsigned tokens are digits only.
 * Outputs remain unchanged on refusal, including overflow. */
UmiStatus umi_ascii_uint64_parse(const char *text, size_t length, uint64_t *out);
UmiStatus umi_ascii_int64_parse(const char *text, size_t length, int64_t *out);
/* Decimal spelling is [+-]digits[.digits][e[+-]digits]. The locale is always C.
 * Nonfinite results and underflow to zero are refused; finite subnormals remain supported. This is
 * binary floating point, not an exact monetary amount parser. Windows UCRT and POSIX locale APIs
 * are supported; other hosts return NOT_IMPLEMENTED without using global locale. */
UmiStatus umi_ascii_double_parse(const char *text, size_t length, double *out);
/* Emit DBL_DECIMAL_DIG significant digits and a dot separator. Output and length
 * remain unchanged on refusal. Capacity includes the terminating NUL. */
UmiStatus umi_ascii_double_format(double value, char *out, size_t capacity, size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
