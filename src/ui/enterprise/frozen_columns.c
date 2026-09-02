/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/frozen_columns.c
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/frozen_columns.h"
#include <string.h>
/*
 * Initialise ui ent frozen columns from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->leading_count=0;value->trailing_count=0;value->total_columns=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent frozen columns satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value){return value!=NULL&&value->leading_count+value->trailing_count<=value->total_columns;}
