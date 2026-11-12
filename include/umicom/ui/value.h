/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/value.h
 *
 * PURPOSE:
 *   Define an owned, ABI-stable value type used by UI properties, bindings and
 *   frontend-neutral view-state snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_VALUE_H
#define UMICOM_UI_VALUE_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_VALUE_STRING_CAPACITY 512U

/**
 * List the named ui value kind values accepted by this public contract.
 */
typedef enum UmiUiValueKind {
    UMI_UI_VALUE_NONE = 0,
    UMI_UI_VALUE_BOOLEAN = 1,
    UMI_UI_VALUE_INTEGER = 2,
    UMI_UI_VALUE_REAL = 3,
    UMI_UI_VALUE_STRING = 4
} UmiUiValueKind;

/**
 * Represent the ui value data shared with callers of this public contract.
 */
typedef struct UmiUiValue {
    UmiUiValueKind kind;
    int boolean_value;
    int64_t integer_value;
    double real_value;
    char string_value[UMI_UI_VALUE_STRING_CAPACITY];
} UmiUiValue;

/**
 * Release or reset state held by ui value so the same storage can be reused safely.
 */
void umi_ui_value_clear(UmiUiValue *value);

/* Validate a defined kind and, for STRING, a NUL inside the fixed buffer.
 * Other active scalar payloads retain their previous domain. This does not
 * validate UTF-8, impose numeric policy or inspect inactive payload bytes. */
int umi_ui_value_valid(const UmiUiValue *value);
/* Check the declared kind set without constructing a payload. NONE is valid. */
int umi_ui_value_kind_valid(UmiUiValueKind kind);
/**
 * Provide the ui value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_boolean(UmiUiValue *value, int enabled);
/**
 * Provide the ui value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_integer(UmiUiValue *value, int64_t number);
/* Exact unsigned observation value using the existing ABI-stable kinds.
 * Values through INT64_MAX use INTEGER; larger values use canonical base-10
 * STRING with no sign, grouping or leading zeroes. Never rounds through REAL.
 * Consumers must accept both kinds for a property using this contract. This
 * formats an identity; it does not grant arithmetic or mutation authority. */
UmiStatus UmiUiValueSetUnsigned(UmiUiValue *value, uint64_t number);
/**
 * Provide the ui value set real operation used by this module and its client applications.
 */
UmiStatus umi_ui_value_set_real(UmiUiValue *value, double number);
/**
 * Provide the ui value set string operation used by this module and its client
 * applications.
 */
/* Copy at most STRING_CAPACITY - 1 bytes plus NUL. Input must be readable
 * through its terminator or STRING_CAPACITY bytes. Self text and suffixes are
 * supported. NULL returns INVALID_ARGUMENT; missing bounded NUL returns
 * CAPACITY_EXCEEDED. Every refusal leaves the entire destination unchanged. */
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text);
/**
 * Copy ui value into module-owned storage so callers keep ownership of their input values.
 */
/* Reject malformed source records with INVALID_ARGUMENT before mutation.
 * Valid complete self-copy is supported; inactive bytes are copied as before. */
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source);
/**
 * Provide the ui value equal operation used by this module and its client applications.
 */
/* Invalid records compare unequal, including invalid self-comparisons.
 * NONE compares equal; scalar comparison semantics are unchanged (NaN is
 * unequal to itself). STRING comparison uses bytes, not Unicode equivalence. */
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right);

#ifdef __cplusplus
}
#endif

#endif
