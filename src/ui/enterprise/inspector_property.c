/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_property.c
 *
 * PURPOSE:
 *   Describe an editable property row for enterprise inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_property.h"
#include <string.h>
/*
 * Initialise ui ent inspector property from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_property_init(UmiUiEntInspectorProperty *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->property_id[0]='\0';value->section_id[0]='\0';value->label[0]='\0';value->value[0]='\0';value->value_type[0]='\0';value->editable=0;value->required=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector property satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_inspector_property_validate(const UmiUiEntInspectorProperty *value){return value!=NULL&&umi_ui_ent_id_valid(value->property_id)&&umi_ui_ent_id_valid(value->section_id)&&value->label[0]!='\0';}
