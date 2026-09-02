/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/component.h
 *
 * PURPOSE:
 *   Describe one semantic UI/application component independently of GTK4, Qt, web or headless renderers.
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

#ifndef UMICOM_DECLARATIVE_COMPONENT_H
#define UMICOM_DECLARATIVE_COMPONENT_H

#include "umicom/declarative/property.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the decl component descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiDeclComponentDescriptor {
    char component_type[UMI_DECL_ID_CAPACITY];
    char display_name[UMI_DECL_NAME_CAPACITY];
    char category[UMI_DECL_NAME_CAPACITY];
    int container;
    UmiDeclPropertyDescriptor properties[UMI_DECL_MAX_PROPERTIES];
    size_t property_count;
} UmiDeclComponentDescriptor;

/**
 * Initialise decl component descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_decl_component_descriptor_init(UmiDeclComponentDescriptor *descriptor, const char *component_type, const char *display_name, const char *category, int container);
/**
 * Provide the decl component add property operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_component_add_property(UmiDeclComponentDescriptor *descriptor, const UmiDeclPropertyDescriptor *property);
/**
 * Provide the decl component find property operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_component_find_property(const UmiDeclComponentDescriptor *descriptor, const char *name, UmiDeclPropertyDescriptor *out_property);

#ifdef __cplusplus
}
#endif

#endif
