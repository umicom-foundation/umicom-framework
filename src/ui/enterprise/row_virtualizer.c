/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_virtualizer.c
 *
 * PURPOSE:
 *   Implement row virtualisation range updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_virtualizer.h"
UmiStatus umi_ui_ent_row_virtualizer_update(UmiUiEntRowVirtualizer *v,const UmiUiEntViewport *vp,size_t total){if(!v||!vp)return UMI_STATUS_INVALID_ARGUMENT;v->total=total;v->materialised=umi_ui_ent_viewport_row_span(vp,total);v->generation++;return UMI_STATUS_OK;}
