/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/attribute.h
 *
 * PURPOSE:
 *   Define named component attributes used by declarative nodes and property inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_ATTRIBUTE_H
#define UMICOM_DECLARATIVE_ATTRIBUTE_H

#include "umicom/declarative/value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclAttribute {
    char name[UMI_DECL_NAME_CAPACITY];
    UmiDeclValue value;
} UmiDeclAttribute;

UmiStatus umi_decl_attribute_init(UmiDeclAttribute *attribute, const char *name, UmiDeclValueKind kind, const char *value_text);
int umi_decl_attribute_name_equal(const UmiDeclAttribute *attribute, const char *name);

#ifdef __cplusplus
}
#endif

#endif
