/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_editor.c
 *
 * PURPOSE:
 *   Describe the semantic editor used for an inspector property.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_editor.h"
#include <string.h>
UmiStatus umi_ui_ent_inspector_editor_init(UmiUiEntInspectorEditor *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->property_id[0]='\0';value->editor_kind[0]='\0';value->choice_count=0;value->multiline=0;value->read_only=0;return UMI_STATUS_OK;}
int umi_ui_ent_inspector_editor_validate(const UmiUiEntInspectorEditor *value){return value!=NULL&&umi_ui_ent_id_valid(value->property_id)&&umi_ui_ent_id_valid(value->editor_kind);}
