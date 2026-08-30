/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_descriptor.c
 *
 * PURPOSE:
 *   Describe a stable enterprise row identity, display label and revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_descriptor.h"
#include <string.h>
UmiStatus umi_ui_ent_row_descriptor_init(UmiUiEntRowDescriptor *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->row_key=0;value->label[0]='\0';value->selectable=0;value->editable=0;value->enabled=0;value->revision=0;value->selectable=1;value->enabled=1;return UMI_STATUS_OK;}
int umi_ui_ent_row_descriptor_validate(const UmiUiEntRowDescriptor *value){return value!=NULL&&value->row_key!=0U;}
