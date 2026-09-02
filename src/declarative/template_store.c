/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/template_store.c
 *
 * PURPOSE:
 *   Implement owned in-memory template storage for starters and designer component fragments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/template_store.h"
#include <stdlib.h>
#include <string.h>
typedef struct Item{char id[UMI_DECL_ID_CAPACITY];char *source;}Item;struct UmiDeclTemplateStore{Item items[UMI_DECL_MAX_TEMPLATES];size_t count;};
/*
 * Initialise decl template store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_decl_template_store_create(UmiDeclTemplateStore **out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=calloc(1U,sizeof(UmiDeclTemplateStore));return *out!=NULL?UMI_STATUS_OK:UMI_STATUS_OUT_OF_MEMORY;}
/*
 * Release or reset state held by decl template store so the same storage can be reused
 * safely.
 */
void umi_decl_template_store_destroy(UmiDeclTemplateStore *s){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)free(s->items[i].source);free(s);}
/*
 * Provide the decl template store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_put(UmiDeclTemplateStore *s,const char *id,const char *src){size_t i;char *copy;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_decl_id_is_valid(id)||src==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->items[i].id,id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_DECL_MAX_TEMPLATES)return UMI_STATUS_CAPACITY_EXCEEDED;copy=malloc(strlen(src)+1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(copy==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)strcpy(copy,src);(void)umi_decl_copy_text(s->items[s->count].id,sizeof(s->items[s->count].id),id);s->items[s->count].source=copy;s->count+=1U;return UMI_STATUS_OK;}
/*
 * Provide the decl template store get operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_get(const UmiDeclTemplateStore *s,const char *id,const char **out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->items[i].id,id)==0){*out=s->items[i].source;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/*
 * Return the number of records represented by decl template store without changing their
 * state.
 */
size_t umi_decl_template_store_count(const UmiDeclTemplateStore *s){return s!=NULL?s->count:0U;}
