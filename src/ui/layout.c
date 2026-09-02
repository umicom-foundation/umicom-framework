/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/layout.c
 *
 * PURPOSE:
 *   Implement a bounded workbench layout graph with parent validation and revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiLayout { UmiUiLayoutNode nodes[UMI_UI_LAYOUT_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Provide the find node operation used by this module and its client applications. */
static size_t find_node(const UmiUiLayout *l,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(l->nodes[i].node_id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui layout from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_layout_create(UmiUiLayout **out){UmiUiLayout *l;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;l=calloc(1U,sizeof(*l));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&l->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(l);return s;}l->revision=1U;*out=l;return UMI_STATUS_OK;}
/* Release or reset state held by ui layout so the same storage can be reused safely. */
void umi_ui_layout_destroy(UmiUiLayout *l){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL)return;umi_mutex_destroy(l->mutex);free(l);}
/* Provide the ui layout upsert operation used by this module and its client applications. */
UmiStatus umi_ui_layout_upsert(UmiUiLayout *l,const UmiUiLayoutNode *n){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||n==NULL||!umi_ui_id_is_valid(n->node_id)||n->ratio<0.0||n->ratio>1.0)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,n->node_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->count>=UMI_UI_LAYOUT_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=l->count++;}l->nodes[i]=*n;l->revision=umi_ui_next_revision(l->revision);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
/* Remove ui layout while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_ui_layout_remove(UmiUiLayout *l,const char *id){size_t i,j;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}/* Visit each bounded item once so every record receives the same rule. */ for(j=0U;j<l->count;++j)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(l->nodes[j].parent_id,id)==0){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<l->count)(void)memmove(&l->nodes[i],&l->nodes[i+1U],(l->count-i-1U)*sizeof(l->nodes[0]));l->count--;l->revision=umi_ui_next_revision(l->revision);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
/* Find ui layout while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_ui_layout_find(const UmiUiLayout *l,const char *id,UmiUiLayoutNode *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}*out=l->nodes[i];(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
/* Find ui layout while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_ui_layout_at(const UmiUiLayout *l,size_t index,UmiUiLayoutNode *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=l->count){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}*out=l->nodes[index];(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
/* Return the number of records represented by ui layout without changing their state. */
size_t umi_ui_layout_count(const UmiUiLayout *l){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l!=NULL){(void)umi_mutex_lock(l->mutex);n=l->count;(void)umi_mutex_unlock(l->mutex);}return n;}
/*
 * Provide the ui layout revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_layout_revision(const UmiUiLayout *l){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l!=NULL){(void)umi_mutex_lock(l->mutex);r=l->revision;(void)umi_mutex_unlock(l->mutex);}return r;}
/* Check that ui layout satisfies its contract before another service relies on it. */
UmiStatus umi_ui_layout_validate(const UmiUiLayout *l,char *out,size_t cap){size_t i;int root_count=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<l->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->nodes[i].kind==UMI_UI_LAYOUT_ROOT)root_count++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(l->nodes[i].parent_id[0]!='\0'&&find_node(l,l->nodes[i].parent_id)==SIZE_MAX){(void)snprintf(out,cap,"Missing parent for %s",l->nodes[i].node_id);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(root_count!=1){(void)snprintf(out,cap,"Expected one root, found %d",root_count);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}(void)snprintf(out,cap,"Layout valid: %zu nodes",l->count);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
