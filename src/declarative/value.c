/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/value.c
 *
 * PURPOSE:
 *   Parse and format declarative values without depending on a GUI toolkit or external parser library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/value.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the decl value from text operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_value_from_text(UmiDeclValueKind kind, const char *text, UmiDeclValue *out_value)
{
    char *end = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_value, 0, sizeof(*out_value));
    out_value->kind = kind;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_copy_text(out_value->text, sizeof(out_value->text), text) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Parse once at the boundary so consumers do not repeatedly reinterpret strings. */
    if (kind == UMI_DECL_VALUE_BOOLEAN) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(text, "true") == 0 || strcmp(text, "1") == 0) out_value->boolean_value = 1;
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "false") == 0 || strcmp(text, "0") == 0) out_value->boolean_value = 0;
        /* Use this fallback path when the earlier condition does not apply. */
        else return UMI_STATUS_PARSE_ERROR;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_DECL_VALUE_INTEGER) {
        errno = 0;
        out_value->integer_value = strtoll(text, &end, 10);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (errno != 0 || end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_DECL_VALUE_REAL) {
        errno = 0;
        out_value->real_value = strtod(text, &end);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (errno != 0 || end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kind != UMI_DECL_VALUE_STRING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the decl value as text operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_value_as_text(const UmiDeclValue *value, char *out_text, size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_DECL_VALUE_BOOLEAN) {
        return umi_decl_copy_text(out_text, capacity, value->boolean_value ? "true" : "false");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_DECL_VALUE_INTEGER) {
        written = snprintf(out_text, capacity, "%lld", (long long)value->integer_value);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (value->kind == UMI_DECL_VALUE_REAL) {
        written = snprintf(out_text, capacity, "%.12g", value->real_value);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return umi_decl_copy_text(out_text, capacity, value->text);
    }
    return written >= 0 && (size_t)written < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the decl value equal operation used by this module and its client applications. */
int umi_decl_value_equal(const UmiDeclValue *left, const UmiDeclValue *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || left->kind != right->kind) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->kind == UMI_DECL_VALUE_BOOLEAN) return left->boolean_value == right->boolean_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->kind == UMI_DECL_VALUE_INTEGER) return left->integer_value == right->integer_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->kind == UMI_DECL_VALUE_REAL) return left->real_value == right->real_value;
    return strcmp(left->text, right->text) == 0;
}
