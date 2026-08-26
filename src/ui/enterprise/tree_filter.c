/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_filter.c
 *
 * PURPOSE:
 *   Implement case-aware tree label filtering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_filter.h"
#include <string.h>
#include <ctype.h>
static int containsi(const char*a,const char*b){size_t i,j;if(!*b)return 1;for(i=0;a[i];i++){for(j=0;b[j]&&a[i+j]&&tolower((unsigned char)a[i+j])==tolower((unsigned char)b[j]);j++);if(!b[j])return 1;}return 0;}
UmiStatus umi_ui_ent_tree_filter_set(UmiUiEntTreeFilter *f,const char*q,int cs,int ancestors){if(!f||!q)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_ent_copy_text(f->query,sizeof f->query,q)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;f->case_sensitive=cs?1:0;f->retain_ancestors=ancestors?1:0;return UMI_STATUS_OK;}
int umi_ui_ent_tree_filter_matches(const UmiUiEntTreeFilter *f,const UmiUiEntTreeNode *n){if(!f||!n)return 0;if(f->query[0]=='\0')return 1;return f->case_sensitive?strstr(n->label,f->query)!=NULL:containsi(n->label,f->query);}
