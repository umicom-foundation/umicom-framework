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
static size_t find_node(const UmiUiLayout *l,const char *id){size_t i;for(i=0U;i<l->count;++i)if(strcmp(l->nodes[i].node_id,id)==0)return i;return SIZE_MAX;}
UmiStatus umi_ui_layout_create(UmiUiLayout **out){UmiUiLayout *l;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;l=calloc(1U,sizeof(*l));if(l==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&l->mutex);if(s!=UMI_STATUS_OK){free(l);return s;}l->revision=1U;*out=l;return UMI_STATUS_OK;}
void umi_ui_layout_destroy(UmiUiLayout *l){if(l==NULL)return;umi_mutex_destroy(l->mutex);free(l);}
UmiStatus umi_ui_layout_upsert(UmiUiLayout *l,const UmiUiLayoutNode *n){size_t i;if(l==NULL||n==NULL||!umi_ui_id_is_valid(n->node_id)||n->ratio<0.0||n->ratio>1.0)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,n->node_id);if(i==SIZE_MAX){if(l->count>=UMI_UI_LAYOUT_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=l->count++;}l->nodes[i]=*n;l->revision=umi_ui_next_revision(l->revision);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_layout_remove(UmiUiLayout *l,const char *id){size_t i,j;if(l==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}for(j=0U;j<l->count;++j)if(strcmp(l->nodes[j].parent_id,id)==0){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}if(i+1U<l->count)(void)memmove(&l->nodes[i],&l->nodes[i+1U],(l->count-i-1U)*sizeof(l->nodes[0]));l->count--;l->revision=umi_ui_next_revision(l->revision);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_layout_find(const UmiUiLayout *l,const char *id,UmiUiLayoutNode *out){size_t i;if(l==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);i=find_node(l,id);if(i==SIZE_MAX){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}*out=l->nodes[i];(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_layout_at(const UmiUiLayout *l,size_t index,UmiUiLayoutNode *out){if(l==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);if(index>=l->count){(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_NOT_FOUND;}*out=l->nodes[index];(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
size_t umi_ui_layout_count(const UmiUiLayout *l){size_t n=0U;if(l!=NULL){(void)umi_mutex_lock(l->mutex);n=l->count;(void)umi_mutex_unlock(l->mutex);}return n;}
uint64_t umi_ui_layout_revision(const UmiUiLayout *l){uint64_t r=0U;if(l!=NULL){(void)umi_mutex_lock(l->mutex);r=l->revision;(void)umi_mutex_unlock(l->mutex);}return r;}
UmiStatus umi_ui_layout_validate(const UmiUiLayout *l,char *out,size_t cap){size_t i;int root_count=0;if(l==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(l->mutex);for(i=0U;i<l->count;++i){if(l->nodes[i].kind==UMI_UI_LAYOUT_ROOT)root_count++;if(l->nodes[i].parent_id[0]!='\0'&&find_node(l,l->nodes[i].parent_id)==SIZE_MAX){(void)snprintf(out,cap,"Missing parent for %s",l->nodes[i].node_id);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}}if(root_count!=1){(void)snprintf(out,cap,"Expected one root, found %d",root_count);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_INVALID_STATE;}(void)snprintf(out,cap,"Layout valid: %zu nodes",l->count);(void)umi_mutex_unlock(l->mutex);return UMI_STATUS_OK;}
