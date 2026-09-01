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

UmiStatus umi_decl_value_from_text(UmiDeclValueKind kind, const char *text, UmiDeclValue *out_value)
{
    char *end = NULL;
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_value, 0, sizeof(*out_value));
    out_value->kind = kind;
    if (umi_decl_copy_text(out_value->text, sizeof(out_value->text), text) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Parse once at the boundary so consumers do not repeatedly reinterpret strings. */
    if (kind == UMI_DECL_VALUE_BOOLEAN) {
        if (strcmp(text, "true") == 0 || strcmp(text, "1") == 0) out_value->boolean_value = 1;
        else if (strcmp(text, "false") == 0 || strcmp(text, "0") == 0) out_value->boolean_value = 0;
        else return UMI_STATUS_PARSE_ERROR;
    } else if (kind == UMI_DECL_VALUE_INTEGER) {
        errno = 0;
        out_value->integer_value = strtoll(text, &end, 10);
        if (errno != 0 || end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    } else if (kind == UMI_DECL_VALUE_REAL) {
        errno = 0;
        out_value->real_value = strtod(text, &end);
        if (errno != 0 || end == text || *end != '\0') return UMI_STATUS_PARSE_ERROR;
    } else if (kind != UMI_DECL_VALUE_STRING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_value_as_text(const UmiDeclValue *value, char *out_text, size_t capacity)
{
    int written;
    if (value == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->kind == UMI_DECL_VALUE_BOOLEAN) {
        return umi_decl_copy_text(out_text, capacity, value->boolean_value ? "true" : "false");
    }
    if (value->kind == UMI_DECL_VALUE_INTEGER) {
        written = snprintf(out_text, capacity, "%lld", (long long)value->integer_value);
    } else if (value->kind == UMI_DECL_VALUE_REAL) {
        written = snprintf(out_text, capacity, "%.12g", value->real_value);
    } else {
        return umi_decl_copy_text(out_text, capacity, value->text);
    }
    return written >= 0 && (size_t)written < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

int umi_decl_value_equal(const UmiDeclValue *left, const UmiDeclValue *right)
{
    if (left == NULL || right == NULL || left->kind != right->kind) return 0;
    if (left->kind == UMI_DECL_VALUE_BOOLEAN) return left->boolean_value == right->boolean_value;
    if (left->kind == UMI_DECL_VALUE_INTEGER) return left->integer_value == right->integer_value;
    if (left->kind == UMI_DECL_VALUE_REAL) return left->real_value == right->real_value;
    return strcmp(left->text, right->text) == 0;
}
