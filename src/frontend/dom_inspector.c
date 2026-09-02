/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/dom_inspector.c
 *
 * PURPOSE:
 *   Implement the dom inspector behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/dom_inspector.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendDomInspector *i,const char *id){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(n=0U;n<i->count;++n)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(i->nodes[n].id,id)==0)return n;return SIZE_MAX;}
/*
 * Initialise frontend dom inspector from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_dom_inspector_init(UmiFrontendDomInspector *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(i,0,sizeof(*i));i->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend dom inspector upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dom_inspector_upsert(UmiFrontendDomInspector *i,const UmiFrontendDomNode *n){size_t p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||n==NULL||n->id[0]=='\0'||n->tag[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;p=find_index(i,n->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->count>=UMI_FRONTEND_DEV_MAX_DOM_NODES)return UMI_STATUS_CAPACITY_EXCEEDED;p=i->count++;}i->nodes[p]=*n;i->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend dom inspector select operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dom_inspector_select(UmiFrontendDomInspector *i,const char *id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_index(i,id)==SIZE_MAX)return UMI_STATUS_NOT_FOUND;s=umi_frontend_dev_copy_text(i->selected_id,sizeof(i->selected_id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)i->revision+=1U;return s;}
/*
 * Find frontend dom inspector while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_dom_inspector_selected(const UmiFrontendDomInspector *i,UmiFrontendDomNode *out){size_t p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=find_index(i,i->selected_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=i->nodes[p];return UMI_STATUS_OK;}
