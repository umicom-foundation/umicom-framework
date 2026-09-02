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
/*
 * Initialise ui ent summary footer from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_ent_summary_footer_init(UmiUiEntSummaryFooter *f){/* Apply this branch only when its contract condition is satisfied. */ if(f)memset(f,0,sizeof *f);}
/*
 * Copy ui ent summary footer into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_summary_footer_set(UmiUiEntSummaryFooter *f,const char *id,double v){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!f||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<f->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(f->items[i].id,id)==0){f->items[i].value=v;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(f->count>=UMI_UI_ENT_MAX_RULES)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(f->items[f->count].id,sizeof f->items[f->count].id,id);f->items[f->count].value=v;f->count++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent summary footer get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_summary_footer_get(const UmiUiEntSummaryFooter *f,const char *id,double *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!f||!id||!out)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<f->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(f->items[i].id,id)==0){*out=f->items[i].value;return 1;}return 0;}
