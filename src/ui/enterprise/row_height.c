/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_height.c
 *
 * PURPOSE:
 *   Describe fixed or adaptive row-height constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_height.h"
#include <string.h>
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->preferred=0;value->minimum=0;value->maximum=0;value->automatic=0;value->preferred=28;value->minimum=18;value->maximum=128;return UMI_STATUS_OK;}
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value){return value!=NULL&&value->minimum>0&&value->maximum>=value->minimum&&value->preferred>=value->minimum&&value->preferred<=value->maximum;}
