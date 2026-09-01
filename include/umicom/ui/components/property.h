/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/property.h
 *
 * PURPOSE:
 *   Store small named component properties used by adapters and designers.
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

#ifndef UMICOM_UI_COMPONENTS_PROPERTY_H
#define UMICOM_UI_COMPONENTS_PROPERTY_H
#include "umicom/base/status.h"
#define UMI_UI_COMPONENT_PROPERTY_KEY_CAPACITY 96U
#define UMI_UI_COMPONENT_PROPERTY_VALUE_CAPACITY 256U
typedef struct UmiUiComponentProperty { char key[UMI_UI_COMPONENT_PROPERTY_KEY_CAPACITY]; char value[UMI_UI_COMPONENT_PROPERTY_VALUE_CAPACITY]; } UmiUiComponentProperty;
UmiStatus umi_ui_component_property_set(UmiUiComponentProperty *property,const char *key,const char *value);
#endif
