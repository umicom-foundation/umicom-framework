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
/*
 * Initialise ui ent row height from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->preferred=0;value->minimum=0;value->maximum=0;value->automatic=0;value->preferred=28;value->minimum=18;value->maximum=128;return UMI_STATUS_OK;}
/* Check that ui ent row height satisfies its contract before another service relies on it. */
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value){return value!=NULL&&value->minimum>0&&value->maximum>=value->minimum&&value->preferred>=value->minimum&&value->preferred<=value->maximum;}
