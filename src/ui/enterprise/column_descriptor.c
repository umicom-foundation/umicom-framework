/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_descriptor.c
 *
 * PURPOSE:
 *   Describe an enterprise grid column with sizing, edit and interaction capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_descriptor.h"
#include <string.h>
/*
 * Initialise ui ent column descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_column_descriptor_init(UmiUiEntColumnDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->label[0]='\0';value->width=0;value->minimum_width=0;value->maximum_width=0;value->sortable=0;value->filterable=0;value->editable=0;value->resizable=0;value->visible=0;value->frozen=0;value->width=120;value->minimum_width=32;value->maximum_width=2048;value->sortable=1;value->filterable=1;value->resizable=1;value->visible=1;return UMI_STATUS_OK;}
/*
 * Check that ui ent column descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_column_descriptor_validate(const UmiUiEntColumnDescriptor *value){return value!=NULL&&umi_ui_ent_id_valid(value->column_id)&&value->minimum_width>0&&value->maximum_width>=value->minimum_width&&value->width>=value->minimum_width&&value->width<=value->maximum_width;}
