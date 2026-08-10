/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/project.c
 *
 * PURPOSE:
 *   Own a bounded collection of declarative designer documents for reusable multi-document authoring hosts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/project.h"
#include <stdlib.h>
#include <string.h>
typedef struct Item{char id[UMI_DECL_ID_CAPACITY];UmiDesignerDocument *doc;int own;}Item;struct UmiDesignerProject{char id[UMI_DECL_ID_CAPACITY];Item items[UMI_DESIGNER_MAX_PROJECT_DOCUMENTS];size_t count;};
UmiStatus umi_designer_project_create(const char *id,UmiDesignerProject **out){UmiDesignerProject *p;if(!umi_decl_id_is_valid(id)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)umi_decl_copy_text(p->id,sizeof(p->id),id);*out=p;return UMI_STATUS_OK;}
void umi_designer_project_destroy(UmiDesignerProject *p){size_t i;if(p==NULL)return;for(i=0U;i<p->count;++i)if(p->items[i].own)umi_designer_document_destroy(p->items[i].doc);free(p);}
UmiStatus umi_designer_project_add(UmiDesignerProject *p,const char *id,UmiDesignerDocument *doc,int own){UmiDesignerDocument *existing=NULL;if(p==NULL||!umi_decl_id_is_valid(id)||doc==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_designer_project_find(p,id,&existing)==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;if(p->count>=UMI_DESIGNER_MAX_PROJECT_DOCUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;(void)umi_decl_copy_text(p->items[p->count].id,sizeof(p->items[p->count].id),id);p->items[p->count].doc=doc;p->items[p->count].own=own!=0;p->count+=1U;return UMI_STATUS_OK;}
UmiStatus umi_designer_project_find(UmiDesignerProject *p,const char *id,UmiDesignerDocument **out){size_t i;if(p==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<p->count;++i)if(strcmp(p->items[i].id,id)==0){*out=p->items[i].doc;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
size_t umi_designer_project_count(const UmiDesignerProject *p){return p!=NULL?p->count:0U;}
