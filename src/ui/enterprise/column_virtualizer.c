/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_virtualizer.c
 *
 * PURPOSE:
 *   Implement column virtualisation range updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_virtualizer.h"
/*
 * Provide the ui ent column virtualizer update operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_ent_column_virtualizer_update(UmiUiEntColumnVirtualizer *v,const UmiUiEntViewport *vp,size_t total){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!v||!vp)return UMI_STATUS_INVALID_ARGUMENT;v->total=total;v->materialised=umi_ui_ent_viewport_column_span(vp,total);v->generation++;return UMI_STATUS_OK;}
