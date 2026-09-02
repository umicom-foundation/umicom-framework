/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/factory.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral callback interface for frontend component factories.
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

#ifndef UMICOM_UI_COMPONENTS_FACTORY_H
#define UMICOM_UI_COMPONENTS_FACTORY_H
#include "umicom/ui/components/component.h"
/**
 * Represent the ui component factory data shared with callers of this public contract.
 */
typedef struct UmiUiComponentFactory { uint32_t structure_size; void *context; UmiStatus (*create)(void *context,const UmiUiComponentSpec *spec,void **out_handle); void (*destroy)(void *context,void *handle); } UmiUiComponentFactory;
/**
 * Check that ui component factory satisfies its contract before another service relies on
 * it.
 */
int umi_ui_component_factory_is_valid(const UmiUiComponentFactory *factory);
#endif
