/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/project.c
 *
 * PURPOSE:
 *   Own a bounded collection of declarative designer documents for reusable multi-document authoring hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/project.h"
#include <stdlib.h>
#include <string.h>
typedef struct Item{char id[UMI_DECL_ID_CAPACITY];UmiDesignerDocument *doc;int own;}Item;struct UmiDesignerProject{char id[UMI_DECL_ID_CAPACITY];Item items[UMI_DESIGNER_MAX_PROJECT_DOCUMENTS];size_t count;};
/*
 * Initialise designer project from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_project_create(const char *id,UmiDesignerProject **out){UmiDesignerProject *p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_decl_id_is_valid(id)||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)umi_decl_copy_text(p->id,sizeof(p->id),id);*out=p;return UMI_STATUS_OK;}
/*
 * Release or reset state held by designer project so the same storage can be reused
 * safely.
 */
void umi_designer_project_destroy(UmiDesignerProject *p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->items[i].own)umi_designer_document_destroy(p->items[i].doc);free(p);}
/* Add designer project only after its inputs and available capacity have been checked. */
UmiStatus umi_designer_project_add(UmiDesignerProject *p,const char *id,UmiDesignerDocument *doc,int own){UmiDesignerDocument *existing=NULL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||!umi_decl_id_is_valid(id)||doc==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_designer_project_find(p,id,&existing)==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_DESIGNER_MAX_PROJECT_DOCUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;(void)umi_decl_copy_text(p->items[p->count].id,sizeof(p->items[p->count].id),id);p->items[p->count].doc=doc;p->items[p->count].own=own!=0;p->count+=1U;return UMI_STATUS_OK;}
/*
 * Find designer project while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_designer_project_find(UmiDesignerProject *p,const char *id,UmiDesignerDocument **out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->items[i].id,id)==0){*out=p->items[i].doc;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/*
 * Return the number of records represented by designer project without changing their
 * state.
 */
size_t umi_designer_project_count(const UmiDesignerProject *p){return p!=NULL?p->count:0U;}
