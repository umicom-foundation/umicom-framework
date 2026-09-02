/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/component_registry.h
 *
 * PURPOSE:
 *   Store semantic component metadata from Framework modules and plug-ins in one deterministic registry.
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

#ifndef UMICOM_DECLARATIVE_COMPONENT_REGISTRY_H
#define UMICOM_DECLARATIVE_COMPONENT_REGISTRY_H

#include "umicom/declarative/component.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the decl component registry data shared with callers of this public contract.
 */
typedef struct UmiDeclComponentRegistry UmiDeclComponentRegistry;
/**
 * Initialise decl component registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_decl_component_registry_create(UmiDeclComponentRegistry **out_registry);
/**
 * Release or reset state held by decl component registry so the same storage can be reused
 * safely.
 */
void umi_decl_component_registry_destroy(UmiDeclComponentRegistry *registry);
/**
 * Add decl component registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_decl_component_registry_register(UmiDeclComponentRegistry *registry,const UmiDeclComponentDescriptor *descriptor);
/**
 * Find decl component registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_decl_component_registry_find(const UmiDeclComponentRegistry *registry,const char *component_type,UmiDeclComponentDescriptor *out_descriptor);
/**
 * Find decl component registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_decl_component_registry_at(const UmiDeclComponentRegistry *registry,size_t index,UmiDeclComponentDescriptor *out_descriptor);
/**
 * Return the number of records represented by decl component registry without changing
 * their state.
 */
size_t umi_decl_component_registry_count(const UmiDeclComponentRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
