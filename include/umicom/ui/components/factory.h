/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/factory.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral callback interface for frontend component factories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_FACTORY_H
#define UMICOM_UI_COMPONENTS_FACTORY_H
#include "umicom/ui/components/component.h"
typedef struct UmiUiComponentFactoryV1 { uint32_t structure_size; void *context; UmiStatus (*create)(void *context,const UmiUiComponentSpec *spec,void **out_handle); void (*destroy)(void *context,void *handle); } UmiUiComponentFactoryV1;
int umi_ui_component_factory_is_valid(const UmiUiComponentFactoryV1 *factory);
#endif
