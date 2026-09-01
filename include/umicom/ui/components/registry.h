/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/registry.h
 *
 * PURPOSE:
 *   Register named component specifications without depending on a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_REGISTRY_H
#define UMICOM_UI_COMPONENTS_REGISTRY_H
#include "umicom/ui/components/component.h"
#define UMI_UI_COMPONENT_REGISTRY_CAPACITY 256U
typedef struct UmiUiComponentRegistry { UmiUiComponentSpec items[UMI_UI_COMPONENT_REGISTRY_CAPACITY]; size_t count; } UmiUiComponentRegistry;
void umi_ui_component_registry_init(UmiUiComponentRegistry *registry);
UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,const UmiUiComponentSpec *spec);
UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec);
#endif
