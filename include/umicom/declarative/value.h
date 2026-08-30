/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/value.h
 *
 * PURPOSE:
 *   Represent declarative property values with explicit types and deterministic text conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_VALUE_H
#define UMICOM_DECLARATIVE_VALUE_H

#include "umicom/declarative/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclValue {
    UmiDeclValueKind kind;
    char text[UMI_DECL_TEXT_CAPACITY];
    int boolean_value;
    int64_t integer_value;
    double real_value;
} UmiDeclValue;

UmiStatus umi_decl_value_from_text(UmiDeclValueKind kind, const char *text, UmiDeclValue *out_value);
UmiStatus umi_decl_value_as_text(const UmiDeclValue *value, char *out_text, size_t capacity);
int umi_decl_value_equal(const UmiDeclValue *left, const UmiDeclValue *right);

#ifdef __cplusplus
}
#endif

#endif
