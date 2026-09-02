/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_filter.c
 *
 * PURPOSE:
 *   Implement case-aware tree label filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_filter.h"
#include <string.h>
#include <ctype.h>
/* Provide the containsi operation used by this module and its client applications. */
static int containsi(const char*a,const char*b){size_t i,j;/* Apply this branch only when its contract condition is satisfied. */ if(!*b)return 1;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;a[i];i++){/* Visit each bounded item once so every record receives the same rule. */ for(j=0;b[j]&&a[i+j]&&tolower((unsigned char)a[i+j])==tolower((unsigned char)b[j]);j++);/* Apply this branch only when its contract condition is satisfied. */ if(!b[j])return 1;}return 0;}
/*
 * Copy ui ent tree filter into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ui_ent_tree_filter_set(UmiUiEntTreeFilter *f,const char*q,int cs,int ancestors){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!f||!q)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(f->query,sizeof f->query,q)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->case_sensitive=cs?1:0;f->retain_ancestors=ancestors?1:0;return UMI_STATUS_OK;}
/*
 * Provide the ui ent tree filter matches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_tree_filter_matches(const UmiUiEntTreeFilter *f,const UmiUiEntTreeNode *n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!f||!n)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f->query[0]=='\0')return 1;return f->case_sensitive?strstr(n->label,f->query)!=NULL:containsi(n->label,f->query);}
