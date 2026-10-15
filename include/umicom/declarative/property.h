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

/**
 * Represent the decl property descriptor data shared with callers of this public contract.
 */
typedef struct UmiDeclPropertyDescriptor {
    char name[UMI_DECL_NAME_CAPACITY];
    UmiDeclValueKind kind;
    char default_value[UMI_DECL_TEXT_CAPACITY];
    int required;
} UmiDeclPropertyDescriptor;

/**
 * Initialise decl property descriptor from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_decl_property_descriptor_init(UmiDeclPropertyDescriptor *descriptor, const char *name, UmiDeclValueKind kind, const char *default_value, int required);
/**
 * Provide the decl property validate text operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_property_validate_text(const UmiDeclPropertyDescriptor *descriptor, const char *value_text);

#ifdef __cplusplus
}
#endif

#endif
