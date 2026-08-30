/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/summary_footer.c
 *
 * PURPOSE:
 *   Implement named grid-footer summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/summary_footer.h"
#include <string.h>
void umi_ui_ent_summary_footer_init(UmiUiEntSummaryFooter *f){if(f)memset(f,0,sizeof *f);}
UmiStatus umi_ui_ent_summary_footer_set(UmiUiEntSummaryFooter *f,const char *id,double v){size_t i;if(!f||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<f->count;i++)if(strcmp(f->items[i].id,id)==0){f->items[i].value=v;return UMI_STATUS_OK;}if(f->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(f->items[f->count].id,sizeof f->items[f->count].id,id);f->items[f->count].value=v;f->count++;return UMI_STATUS_OK;}
int umi_ui_ent_summary_footer_get(const UmiUiEntSummaryFooter *f,const char *id,double *out){size_t i;if(!f||!id||!out)return 0;for(i=0;i<f->count;i++)if(strcmp(f->items[i].id,id)==0){*out=f->items[i].value;return 1;}return 0;}
