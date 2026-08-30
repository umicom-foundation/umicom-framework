/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_style.c
 *
 * PURPOSE:
 *   Describe semantic cell presentation independent of a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_style.h"
#include <string.h>
UmiStatus umi_ui_ent_cell_style_init(UmiUiEntCellStyle *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->semantic_role[0]='\0';value->bold=0;value->italic=0;value->alignment=0;value->indent=0;value->alignment=0;return UMI_STATUS_OK;}
int umi_ui_ent_cell_style_validate(const UmiUiEntCellStyle *value){return value!=NULL;}
