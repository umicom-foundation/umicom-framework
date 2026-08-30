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
void umi_ui_ent_pinned_rows_init(UmiUiEntPinnedRows *p){if(p)memset(p,0,sizeof *p);}
int umi_ui_ent_pinned_rows_contains(const UmiUiEntPinnedRows *p,uint64_t k){size_t i;if(!p)return 0;for(i=0;i<p->top_count;i++)if(p->top[i]==k)return 1;for(i=0;i<p->bottom_count;i++)if(p->bottom[i]==k)return 1;return 0;}
static UmiStatus add(uint64_t *a,size_t *n,uint64_t k,const UmiUiEntPinnedRows *p){if(k==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_ent_pinned_rows_contains(p,k))return UMI_STATUS_ALREADY_EXISTS;if(*n>=UMI_UI_ENT_MAX_SELECTIONS)return UMI_STATUS_CAPACITY_EXCEEDED;a[(*n)++]=k;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_pinned_rows_add_top(UmiUiEntPinnedRows *p,uint64_t k){return p?add(p->top,&p->top_count,k,p):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_ui_ent_pinned_rows_add_bottom(UmiUiEntPinnedRows *p,uint64_t k){return p?add(p->bottom,&p->bottom_count,k,p):UMI_STATUS_INVALID_ARGUMENT;}
