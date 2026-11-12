/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/value.c
 *
 * PURPOSE:
 *   Implement owned UI values with deterministic copying and comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/value.h"
#include <string.h>
#include "ui_internal.h"

/* This contract validates the active payload only. Scalar values keep their
 * historical domain (including non-finite REAL observations); applications
 * may impose stricter numeric rules. Inactive bytes never become string input. */
/* Keep declared-kind admission shared by payloads and converter descriptors. */
int umi_ui_value_kind_valid(UmiUiValueKind kind)
{
    switch (kind) {
        case UMI_UI_VALUE_NONE:
        case UMI_UI_VALUE_BOOLEAN:
        case UMI_UI_VALUE_INTEGER:
        case UMI_UI_VALUE_REAL:
        case UMI_UI_VALUE_STRING: return 1;
        default: return 0;
    }
}

/* Share the declared-kind set with reactive descriptors while retaining active-payload semantics. The previous implementation is retained for review. */
#if 0
int umi_ui_value_valid(const UmiUiValue *value)
{
    if (value == NULL) return 0;
    switch (value->kind) {
        case UMI_UI_VALUE_NONE:
        case UMI_UI_VALUE_BOOLEAN:
        case UMI_UI_VALUE_INTEGER:
        case UMI_UI_VALUE_REAL:
            return 1;
        case UMI_UI_VALUE_STRING:
            return memchr(value->string_value, '\0', sizeof(value->string_value)) != NULL;
        default:
            return 0;
    }
}
#endif
int umi_ui_value_valid(const UmiUiValue *value)
{
    if (value == NULL || !umi_ui_value_kind_valid(value->kind)) return 0;
    return value->kind != UMI_UI_VALUE_STRING ||
        memchr(value->string_value, '\0', sizeof(value->string_value)) != NULL;
}



/* Release or reset state held by ui value so the same storage can be reused safely. */
void umi_ui_value_clear(UmiUiValue *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) (void)memset(value, 0, sizeof(*value));
}
/*
 * Provide the ui value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_boolean(UmiUiValue *value, int enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_BOOLEAN;
    value->boolean_value = enabled != 0; return UMI_STATUS_OK;
}
/*
 * Provide the ui value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_integer(UmiUiValue *value, int64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_INTEGER;
    value->integer_value = number; return UMI_STATUS_OK;
}
/* Provide the ui value set real operation used by this module and its client applications. */
UmiStatus umi_ui_value_set_real(UmiUiValue *value, double number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_REAL;
    value->real_value = number; return UMI_STATUS_OK;
}
/*
 * Provide the ui value set string operation used by this module and its client
 * applications.
 */
/* Stage bounded text before replacing the value so failed and aliased edits preserve caller ownership. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_STRING;
    return umi_ui_copy_text(value->string_value, sizeof(value->string_value), text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
#endif
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text)
{
    UmiUiValue candidate = {0};
    size_t length = 0U;
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Read at most the supported byte count, stopping at NUL. Staging before
     * publication supports self-assignment and suffixes of the current text.
     * A refused edit must not erase the caller's previous presentation value. */
    while (length < sizeof(candidate.string_value) && text[length] != '\0')
        ++length;
    if (length == sizeof(candidate.string_value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.kind = UMI_UI_VALUE_STRING;
    memcpy(candidate.string_value, text, length + 1U);
    *value = candidate;
    return UMI_STATUS_OK;
}
/* Copy ui value into module-owned storage so callers keep ownership of their input values. */
/* Admit the active value contract before publishing a copy to the destination. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source)
{
    /* Reuse the same contract at the ownership boundary. Complete valid
     * self-copy is supported; rejected source records leave the owner intact. */
    if (destination == NULL || !umi_ui_value_valid(source))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (destination != source) *destination = *source;
    return UMI_STATUS_OK;
}
/* Provide the ui value equal operation used by this module and its client applications. */
/* Validate active kinds and fixed string boundaries before comparing externally supplied values. The previous implementation is retained for review. */
#if 0
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || left->kind != right->kind) return 0;
    /* Select the behaviour associated with the requested command or state value. */
    switch (left->kind) {
        case UMI_UI_VALUE_BOOLEAN: return left->boolean_value == right->boolean_value;
        case UMI_UI_VALUE_INTEGER: return left->integer_value == right->integer_value;
        case UMI_UI_VALUE_REAL: return left->real_value == right->real_value;
        case UMI_UI_VALUE_STRING: return strcmp(left->string_value, right->string_value) == 0;
        default: return 1;
    }
}
#endif
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right)
{
    /* Fixed text can originate in a plug-in or restored snapshot. Establish
     * both bounded terminators before strcmp; unsupported kinds are not equal
     * merely because both contain the same unrecognized discriminator. */
    if (!umi_ui_value_valid(left) || !umi_ui_value_valid(right) ||
        left->kind != right->kind) return 0;
    switch (left->kind) {
        case UMI_UI_VALUE_NONE: return 1;
        case UMI_UI_VALUE_BOOLEAN: return left->boolean_value == right->boolean_value;
        case UMI_UI_VALUE_INTEGER: return left->integer_value == right->integer_value;
        case UMI_UI_VALUE_REAL: return left->real_value == right->real_value;
        case UMI_UI_VALUE_STRING: return strcmp(left->string_value, right->string_value) == 0;
        default: return 0;
    }
}


/* Preserve the complete uint64 range without extending the ABI-stable value
 * layout or rounding through double. Existing renderers already understand
 * integer and string values. Large identities use canonical decimal text. */
UmiStatus UmiUiValueSetUnsigned(UmiUiValue *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (number <= (uint64_t)INT64_MAX)
        return umi_ui_value_set_integer(value, (int64_t)number);
    char decimal[21];
    size_t offset = sizeof(decimal) - 1U;
    decimal[offset] = '\0';
    do {
        decimal[--offset] = (char)('0' + number % 10U);
        number /= 10U;
    } while (number != 0U);
    return umi_ui_value_set_string(value, decimal + offset);
}
