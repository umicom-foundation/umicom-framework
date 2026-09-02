/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/pinned_rows.c
 *
 * PURPOSE:
 *   Implement bounded pinned-row identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/pinned_rows.h"
#include <string.h>
/*
 * Initialise ui ent pinned rows from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_pinned_rows_init(UmiUiEntPinnedRows *p){/* Apply this branch only when its contract condition is satisfied. */ if(p)memset(p,0,sizeof *p);}
/*
 * Provide the ui ent pinned rows contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_pinned_rows_contains(const UmiUiEntPinnedRows *p,uint64_t k){size_t i;/* Apply this branch only when its contract condition is satisfied. */ if(!p)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<p->top_count;i++)/* Apply this branch only when its contract condition is satisfied. */ if(p->top[i]==k)return 1;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<p->bottom_count;i++)/* Apply this branch only when its contract condition is satisfied. */ if(p->bottom[i]==k)return 1;return 0;}
/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(uint64_t *a,size_t *n,uint64_t k,const UmiUiEntPinnedRows *p){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(k==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_pinned_rows_contains(p,k))return UMI_STATUS_ALREADY_EXISTS;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(*n>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;a[(*n)++]=k;return UMI_STATUS_OK;}
/*
 * Provide the ui ent pinned rows add top operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_pinned_rows_add_top(UmiUiEntPinnedRows *p,uint64_t k){return p?add(p->top,&p->top_count,k,p):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the ui ent pinned rows add bottom operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_pinned_rows_add_bottom(UmiUiEntPinnedRows *p,uint64_t k){return p?add(p->bottom,&p->bottom_count,k,p):UMI_STATUS_INVALID_ARGUMENT;}
