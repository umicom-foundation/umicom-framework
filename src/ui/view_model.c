/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/view_model.c
 *
 * PURPOSE:
 *   Implement owned view-model hierarchy, state changes and property access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/view_model.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiViewModel { char id[UMI_UI_ID_CAPACITY]; char type[UMI_UI_ID_CAPACITY]; char parent[UMI_UI_ID_CAPACITY]; char children[UMI_UI_VIEW_CHILD_MAX][UMI_UI_ID_CAPACITY]; size_t child_count; UmiUiRole role; int visible; int enabled; uint64_t revision; UmiUiPropertyBag *properties; UmiMutex *mutex; };
/* Find child while leaving the underlying catalogue or model owned by this module. */
static size_t child_find(const UmiUiViewModel *view,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<view->child_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(view->children[i],id)==0)return i;return SIZE_MAX;}
/*
 * Initialise ui view model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_view_model_create(const char *id,const char *type,UmiUiRole role,UmiUiViewModel **out_view){UmiUiViewModel *view;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_view==NULL||!umi_ui_id_is_valid(id)||!umi_ui_id_is_valid(type))return UMI_STATUS_INVALID_ARGUMENT;*out_view=NULL;view=calloc(1U,sizeof(*view));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)umi_ui_copy_text(view->id,sizeof(view->id),id);(void)umi_ui_copy_text(view->type,sizeof(view->type),type);view->role=role;view->visible=1;view->enabled=1;view->revision=1U;s=umi_mutex_create(&view->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_ui_property_bag_create(&view->properties);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){umi_mutex_destroy(view->mutex);free(view);return s;}*out_view=view;return UMI_STATUS_OK;}
/* Release or reset state held by ui view model so the same storage can be reused safely. */
void umi_ui_view_model_destroy(UmiUiViewModel *view){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL)return;umi_ui_property_bag_destroy(view->properties);umi_mutex_destroy(view->mutex);free(view);}
/*
 * Provide the ui view model set parent operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_parent(UmiUiViewModel *view,const char *parent_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL||(parent_id!=NULL&&parent_id[0]!='\0'&&!umi_ui_id_is_valid(parent_id)))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_copy_text(view->parent,sizeof(view->parent),parent_id)){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model add child operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_add_child(UmiUiViewModel *view,const char *child_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL||!umi_ui_id_is_valid(child_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(child_find(view,child_id)!=SIZE_MAX){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_ALREADY_EXISTS;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view->child_count>=UMI_UI_VIEW_CHILD_MAX){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_ui_copy_text(view->children[view->child_count++],UMI_UI_ID_CAPACITY,child_id);view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model remove child operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_remove_child(UmiUiViewModel *view,const char *child_id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL||child_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);i=child_find(view,child_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<view->child_count)(void)memmove(&view->children[i],&view->children[i+1U],(view->child_count-i-1U)*sizeof(view->children[0]));view->child_count--;view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model set visible operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_visible(UmiUiViewModel *view,int visible){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);view->visible=visible!=0;view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_enabled(UmiUiViewModel *view,int enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);view->enabled=enabled!=0;view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model set property operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_property(UmiUiViewModel *view,const char *key,const UmiUiValue *value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_ui_property_bag_set(view->properties,key,value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){(void)umi_mutex_lock(view->mutex);view->revision=umi_ui_next_revision(view->revision);(void)umi_mutex_unlock(view->mutex);}return s;}
/*
 * Provide the ui view model get property operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_get_property(const UmiUiViewModel *view,const char *key,UmiUiValue *out_value){return view!=NULL?umi_ui_property_bag_get(view->properties,key,out_value):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the ui view model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_snapshot(const UmiUiViewModel *view,UmiUiViewSnapshot *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);(void)umi_ui_copy_text(out->view_id,sizeof(out->view_id),view->id);(void)umi_ui_copy_text(out->view_type,sizeof(out->view_type),view->type);(void)umi_ui_copy_text(out->parent_id,sizeof(out->parent_id),view->parent);out->role=view->role;out->visible=view->visible;out->enabled=view->enabled;out->revision=view->revision;out->child_count=view->child_count;(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Find ui view model child while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_view_model_child_at(const UmiUiViewModel *view,size_t index,char *out,size_t cap){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(view==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(view->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=view->child_count){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_copy_text(out,cap,view->children[index])){(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_mutex_unlock(view->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui view model properties operation used by this module and its client
 * applications.
 */
UmiUiPropertyBag *umi_ui_view_model_properties(UmiUiViewModel *view){return view!=NULL?view->properties:NULL;}
