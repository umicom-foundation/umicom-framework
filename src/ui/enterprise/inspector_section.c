/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_section.c
 *
 * PURPOSE:
 *   Describe a collapsible property-inspector section.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_section.h"
#include <string.h>
UmiStatus umi_ui_ent_inspector_section_init(UmiUiEntInspectorSection *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->section_id[0]='\0';value->label[0]='\0';value->order=0;value->collapsed=0;return UMI_STATUS_OK;}
int umi_ui_ent_inspector_section_validate(const UmiUiEntInspectorSection *value){return value!=NULL&&umi_ui_ent_id_valid(value->section_id);}
