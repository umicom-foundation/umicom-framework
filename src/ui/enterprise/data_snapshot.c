/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/data_snapshot.c
 *
 * PURPOSE:
 *   Describe an immutable logical data snapshot used by virtual views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_snapshot.h"
#include <string.h>
UmiStatus umi_ui_ent_data_snapshot_init(UmiUiEntDataSnapshot *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->generation=0;value->row_count=0;value->column_count=0;value->complete=0;value->source_revision=0;value->complete=1;return UMI_STATUS_OK;}
int umi_ui_ent_data_snapshot_validate(const UmiUiEntDataSnapshot *value){return value!=NULL&&value->column_count<=UMI_UI_ENT_MAX_COLUMNS;}
