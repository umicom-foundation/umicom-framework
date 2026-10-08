/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/decimal.h
 *
 * PURPOSE:
 *   Represent fixed-scale decimal coefficients for cash and control calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module keeps decimal logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_DECIMAL_H
#define INCLUDE_UMICOM_FINANCE_DECIMAL_H
#include "umicom/base/status.h"
#include <stdbool.h>
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the decimal rescale operation used by this module and its client applications.
 */
/* Source and target scales must be 0..9. Upscaling is checked before
 * multiplication. Reducing the scale preserves truncation towards zero.
 * Failure leaves the caller's output unchanged. */
UmiStatus umi_decimal_rescale(UmiDecimal value, uint8_t target_scale, UmiDecimal *out);

#define UMI_DECIMAL_TEXT_MAX 96U

/** Parse exactly length bytes of fixed-point text into the requested scale.
 * Accepts optional sign and surrounding ASCII spaces/tabs, decimal digits,
 * and an optional point followed by 1..scale digits. "12.5" at scale 2 is
 * coefficient 1250. Group separators, exponent notation, embedded zero and
 * excess decimal places are rejected, never rounded. Scale is explicit (0..9);
 * it is not inferred from a currency code. The input is borrowed only during
 * the call. Output changes only on success; no allocation occurs.
 * See examples/workflow_tools/main.c for a complete banking/treasury example. */
UmiStatus UmiDecimalParse(const char *text, size_t length, uint8_t scale,
                         UmiDecimal *out);

/** Produce signed decimal text with exactly value.scale fractional digits.
 * Handles INT64_MIN without negating it. Output includes a terminating zero.
 * Insufficient capacity returns CAPACITY_EXCEEDED without altering out.
 * Neither parse nor format uses floating-point arithmetic or the current locale. */
UmiStatus UmiDecimalFormat(UmiDecimal value, char *out, size_t capacity);

/** Compare the exact values, including different scales and INT64_MIN. The
 * result is -1, 0 or 1; comparison never rescales into a larger coefficient.
 * An invalid scale leaves the output unchanged. No allocation or locale is used. */
UmiStatus UmiDecimalCompare(UmiDecimal left, UmiDecimal right, int *outComparison);

/** Add at the finer input scale without rounding. If rescaling either operand
 * or adding their coefficients would overflow, return CAPACITY_EXCEEDED and
 * preserve out. This conservative rule also applies when opposite signs might
 * otherwise cancel an unrepresentable intermediate coefficient. */
UmiStatus UmiDecimalAddExact(UmiDecimal left, UmiDecimal right, UmiDecimal *out);


/** Parse a provider's fixed or scientific decimal without floating point.
 * Input has 1..96 bytes, optional sign, digits, an optional fractional part,
 * and an optional signed exponent (magnitude at most 1000). Whitespace,
 * grouping, embedded zero, NaN and infinity are refused. Trailing fractional
 * zeros may be removed; no nonzero digit is rounded. The smallest exact scale
 * in 0..9 is returned. Failure preserves out. */
UmiStatus UmiDecimalParseScientificExact(const char *text, size_t length, UmiDecimal *out);

/** Test an exact multiple of a positive increment, including mixed scales and
 * INT64_MIN. Zero is aligned; negative values use their magnitude. No rescaling
 * can overflow and neither operand is rounded. Invalid input preserves out. */
UmiStatus UmiDecimalIsMultiple(UmiDecimal value, UmiDecimal increment, bool *out);

#ifdef __cplusplus
}
#endif
#endif
