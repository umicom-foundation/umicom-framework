/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_editor.c
 *
 * PURPOSE:
 *   Describe the semantic editor used for an inspector property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_editor.h"
#include <string.h>
/*
 * Initialise ui ent inspector editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_editor_init(UmiUiEntInspectorEditor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->property_id[0]='\0';value->editor_kind[0]='\0';value->choice_count=0;value->multiline=0;value->read_only=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector editor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_editor_validate(const UmiUiEntInspectorEditor *value){return value!=NULL&&umi_ui_ent_id_valid(value->property_id)&&umi_ui_ent_id_valid(value->editor_kind);}
