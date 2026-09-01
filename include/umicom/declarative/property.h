/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/property.h
 *
 * PURPOSE:
 *   Describe typed component properties, defaults and whether a value is required by a component schema.
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

#ifndef UMICOM_DECLARATIVE_PROPERTY_H
#define UMICOM_DECLARATIVE_PROPERTY_H

#include "umicom/declarative/value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeclPropertyDescriptor {
    char name[UMI_DECL_NAME_CAPACITY];
    UmiDeclValueKind kind;
    char default_value[UMI_DECL_TEXT_CAPACITY];
    int required;
} UmiDeclPropertyDescriptor;

UmiStatus umi_decl_property_descriptor_init(UmiDeclPropertyDescriptor *descriptor, const char *name, UmiDeclValueKind kind, const char *default_value, int required);
UmiStatus umi_decl_property_validate_text(const UmiDeclPropertyDescriptor *descriptor, const char *value_text);

#ifdef __cplusplus
}
#endif

#endif
