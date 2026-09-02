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
/**
 * Represent the ui component registry data shared with callers of this public contract.
 */
typedef struct UmiUiComponentRegistry { UmiUiComponentSpec items[UMI_UI_COMPONENT_REGISTRY_CAPACITY]; size_t count; } UmiUiComponentRegistry;
/**
 * Initialise ui component registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_component_registry_init(UmiUiComponentRegistry *registry);
/**
 * Add ui component registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,const UmiUiComponentSpec *spec);
/**
 * Find ui component registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec);
#endif
