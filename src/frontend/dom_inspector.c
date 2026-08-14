/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/dom_inspector.h"
#include <string.h>
static size_t find_index(const UmiFrontendDomInspector *i,const char *id){size_t n;if(i==NULL||id==NULL)return SIZE_MAX;for(n=0U;n<i->count;++n)if(strcmp(i->nodes[n].id,id)==0)return n;return SIZE_MAX;}
UmiStatus umi_frontend_dom_inspector_init(UmiFrontendDomInspector *i){if(i==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(i,0,sizeof(*i));i->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_dom_inspector_upsert(UmiFrontendDomInspector *i,const UmiFrontendDomNode *n){size_t p;if(i==NULL||n==NULL||n->id[0]=='\0'||n->tag[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;p=find_index(i,n->id);if(p==SIZE_MAX){if(i->count>=UMI_FRONTEND_DEV_MAX_DOM_NODES)return UMI_STATUS_CAPACITY_EXCEEDED;p=i->count++;}i->nodes[p]=*n;i->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_dom_inspector_select(UmiFrontendDomInspector *i,const char *id){UmiStatus s;if(i==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(find_index(i,id)==SIZE_MAX)return UMI_STATUS_NOT_FOUND;s=umi_frontend_dev_copy_text(i->selected_id,sizeof(i->selected_id),id);if(s==UMI_STATUS_OK)i->revision+=1U;return s;}
UmiStatus umi_frontend_dom_inspector_selected(const UmiFrontendDomInspector *i,UmiFrontendDomNode *out){size_t p;if(i==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;p=find_index(i,i->selected_id);if(p==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=i->nodes[p];return UMI_STATUS_OK;}
