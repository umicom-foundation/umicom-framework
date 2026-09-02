/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_section.c
 *
 * PURPOSE:
 *   Describe a collapsible property-inspector section.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_section.h"
#include <string.h>
/*
 * Initialise ui ent inspector section from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_section_init(UmiUiEntInspectorSection *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->section_id[0]='\0';value->label[0]='\0';value->order=0;value->collapsed=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector section satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_section_validate(const UmiUiEntInspectorSection *value){return value!=NULL&&umi_ui_ent_id_valid(value->section_id);}
