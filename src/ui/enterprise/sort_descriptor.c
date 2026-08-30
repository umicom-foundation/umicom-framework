/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sort_descriptor.c
 *
 * PURPOSE:
 *   Describe one deterministic multi-column sort key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_descriptor.h"
#include <string.h>
UmiStatus umi_ui_ent_sort_descriptor_init(UmiUiEntSortDescriptor *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->direction=0;value->priority=0;value->case_sensitive=0;return UMI_STATUS_OK;}
int umi_ui_ent_sort_descriptor_validate(const UmiUiEntSortDescriptor *value){return value!=NULL&&umi_ui_ent_id_valid(value->column_id)&&value->direction>=UMI_UI_ENT_SORT_NONE&&value->direction<=UMI_UI_ENT_SORT_DESCENDING;}
