/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/weight_log.h
 *
 * PURPOSE:
 *   Share a non-clinical weight log schema and exact kilogram input across clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_WEIGHT_LOG_H
#define UMICOM_HEALTHCARE_WEIGHT_LOG_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WEIGHT_LOG_MAX_GRAMS UINT32_C(1000000000)
/* This is an encoding capacity, never a recommended or healthy body weight.
 * Date is the user's explicit calendar date, not a derived UTC timestamp.
 * No identity, diagnosis, target, calorie advice or network policy is implied. */
typedef struct UmiWeightLogEntry {
    char observed_on[11];
    uint32_t mass_grams;
    char note[256];
} UmiWeightLogEntry;
/* Immutable schema borrowed for the process lifetime. It uses the shared form
 * editor, date validation, snapshot, web and record services without a new ABI. */
const UmiStructuredFormSchema *umi_weight_log_schema(void);
/* Parse exactly length bytes of ASCII kilograms: 1..7 whole digits, optionally
 * a dot and 1..3 fractional digits. No spaces, signs, exponent, comma, unit
 * suffix, embedded NUL or rounding. Leading zeroes are allowed within the bound.
 * Syntax returns PARSE_ERROR; zero/out-of-capacity returns INVALID_ARGUMENT.
 * Both pointers are required. Output is unchanged on refusal; no allocation. */
UmiStatus umi_weight_log_parse_kilograms(const char *text, size_t length,
                                       uint32_t *out_grams);
/* Validate a complete entry before copying it out. Unanswered optional note
 * becomes empty text; missing date/mass or any invalid form returns an error.
 * Form and output storage must be disjoint and stable for the call. No record
 * is saved or marked clean. Persistence and access control remain host-owned. */
UmiStatus umi_weight_log_read(const UmiStructuredForm *form, UmiWeightLogEntry *out);
#ifdef __cplusplus
}
#endif
#endif
