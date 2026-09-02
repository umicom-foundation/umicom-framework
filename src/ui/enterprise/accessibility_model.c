/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/accessibility_model.c
 *
 * PURPOSE:
 *   Describe accessible row/column metadata for virtualised enterprise views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/accessibility_model.h"
#include <string.h>
/*
 * Initialise ui ent accessibility model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_accessibility_model_init(UmiUiEntAccessibilityModel *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->role[0]='\0';value->name[0]='\0';value->description[0]='\0';value->row_index=0;value->column_index=0;value->set_size=0;value->position_in_set=0;value->position_in_set=1U;return UMI_STATUS_OK;}
/*
 * Check that ui ent accessibility model satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_accessibility_model_validate(const UmiUiEntAccessibilityModel *value){return value!=NULL&&umi_ui_ent_id_valid(value->role)&&value->name[0]!='\0'&&value->position_in_set>0U;}
