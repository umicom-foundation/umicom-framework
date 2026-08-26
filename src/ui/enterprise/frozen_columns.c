/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/frozen_columns.c
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/frozen_columns.h"
#include <string.h>
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->leading_count=0;value->trailing_count=0;value->total_columns=0;return UMI_STATUS_OK;}
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value){return value!=NULL&&value->leading_count+value->trailing_count<=value->total_columns;}
