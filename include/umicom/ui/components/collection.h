/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/collection.h
 *
 * PURPOSE:
 *   Hold ordered component identifiers for lists, menus, tabs and tool surfaces.
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

#ifndef UMICOM_UI_COMPONENTS_COLLECTION_H
#define UMICOM_UI_COMPONENTS_COLLECTION_H
#include "umicom/ui/components/component.h"
#define UMI_UI_COMPONENT_COLLECTION_CAPACITY 256U
/**
 * Represent the ui component collection data shared with callers of this public contract.
 */
typedef struct UmiUiComponentCollection { char ids[UMI_UI_COMPONENT_COLLECTION_CAPACITY][UMI_UI_COMPONENT_ID_CAPACITY]; size_t count; } UmiUiComponentCollection;
/**
 * Initialise ui component collection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_component_collection_init(UmiUiComponentCollection *collection);
/**
 * Add ui component collection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_component_collection_add(UmiUiComponentCollection *collection,const char *id);
#endif
