/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/component_set.h
 *
 * PURPOSE:
 *   Bundle a registry and component tree as one reusable UI composition document.
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

#ifndef UMICOM_UI_COMPONENTS_COMPONENT_SET_H
#define UMICOM_UI_COMPONENTS_COMPONENT_SET_H
#include "umicom/ui/components/registry.h"
#include "umicom/ui/components/tree.h"
/**
 * Represent the ui component set data shared with callers of this public contract.
 */
typedef struct UmiUiComponentSet { UmiUiComponentRegistry registry; UmiUiComponentTree tree; } UmiUiComponentSet;
/**
 * Initialise ui component set from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_component_set_init(UmiUiComponentSet *set);
#endif
