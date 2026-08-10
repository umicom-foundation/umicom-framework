/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/template_store.c
 *
 * PURPOSE:
 *   Implement owned in-memory template storage for starters and designer component fragments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/template_store.h"
#include <stdlib.h>
#include <string.h>
typedef struct Item{char id[UMI_DECL_ID_CAPACITY];char *source;}Item;struct UmiDeclTemplateStore{Item items[UMI_DECL_MAX_TEMPLATES];size_t count;};
UmiStatus umi_decl_template_store_create(UmiDeclTemplateStore **out){if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=calloc(1U,sizeof(UmiDeclTemplateStore));return *out!=NULL?UMI_STATUS_OK:UMI_STATUS_OUT_OF_MEMORY;}
void umi_decl_template_store_destroy(UmiDeclTemplateStore *s){size_t i;if(s==NULL)return;for(i=0U;i<s->count;++i)free(s->items[i].source);free(s);}
UmiStatus umi_decl_template_store_put(UmiDeclTemplateStore *s,const char *id,const char *src){size_t i;char *copy;if(s==NULL||!umi_decl_id_is_valid(id)||src==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->count;++i)if(strcmp(s->items[i].id,id)==0)return UMI_STATUS_ALREADY_EXISTS;if(s->count>=UMI_DECL_MAX_TEMPLATES)return UMI_STATUS_CAPACITY_EXCEEDED;copy=malloc(strlen(src)+1U);if(copy==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)strcpy(copy,src);(void)umi_decl_copy_text(s->items[s->count].id,sizeof(s->items[s->count].id),id);s->items[s->count].source=copy;s->count+=1U;return UMI_STATUS_OK;}
UmiStatus umi_decl_template_store_get(const UmiDeclTemplateStore *s,const char *id,const char **out){size_t i;if(s==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->count;++i)if(strcmp(s->items[i].id,id)==0){*out=s->items[i].source;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
size_t umi_decl_template_store_count(const UmiDeclTemplateStore *s){return s!=NULL?s->count:0U;}
