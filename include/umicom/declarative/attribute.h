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

/**
 * Represent the decl attribute data shared with callers of this public contract.
 */
typedef struct UmiDeclAttribute {
    char name[UMI_DECL_NAME_CAPACITY];
    UmiDeclValue value;
} UmiDeclAttribute;

/**
 * Initialise decl attribute from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_decl_attribute_init(UmiDeclAttribute *attribute, const char *name, UmiDeclValueKind kind, const char *value_text);
/**
 * Provide the decl attribute name equal operation used by this module and its client
 * applications.
 */
int umi_decl_attribute_name_equal(const UmiDeclAttribute *attribute, const char *name);

#ifdef __cplusplus
}
#endif

#endif
