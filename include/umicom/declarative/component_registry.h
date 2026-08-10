/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/component_registry.h
 *
 * PURPOSE:
 *   Store semantic component metadata from Framework modules and plug-ins in one deterministic registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiDeclComponentRegistry UmiDeclComponentRegistry;
UmiStatus umi_decl_component_registry_create(UmiDeclComponentRegistry **out_registry);
void umi_decl_component_registry_destroy(UmiDeclComponentRegistry *registry);
UmiStatus umi_decl_component_registry_register(UmiDeclComponentRegistry *registry,const UmiDeclComponentDescriptor *descriptor);
UmiStatus umi_decl_component_registry_find(const UmiDeclComponentRegistry *registry,const char *component_type,UmiDeclComponentDescriptor *out_descriptor);
UmiStatus umi_decl_component_registry_at(const UmiDeclComponentRegistry *registry,size_t index,UmiDeclComponentDescriptor *out_descriptor);
size_t umi_decl_component_registry_count(const UmiDeclComponentRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
