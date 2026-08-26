/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/grouping_descriptor.c
 *
 * PURPOSE:
 *   Describe a grouping key and default expansion semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/grouping_descriptor.h"
#include <string.h>
UmiStatus umi_ui_ent_grouping_descriptor_init(UmiUiEntGroupingDescriptor *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->level=0;value->expanded_by_default=0;return UMI_STATUS_OK;}
int umi_ui_ent_grouping_descriptor_validate(const UmiUiEntGroupingDescriptor *value){return value!=NULL&&umi_ui_ent_id_valid(value->column_id);}
