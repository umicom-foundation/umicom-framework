/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench.c
 *
 * PURPOSE:
 *   Implement the authoritative workbench aggregate and action-to-command execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workbench.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiWorkbench { char id[UMI_UI_ID_CAPACITY]; char active_perspective[UMI_UI_ID_CAPACITY]; char active_document[UMI_UI_ID_CAPACITY]; uint64_t revision; UmiCommandRegistry *commands; UmiUiPaneModel *panes; UmiUiDocumentViewModel *documents; UmiUiEditorModel *editors; UmiUiPerspectiveModel *perspectives; UmiUiLayout *layout; UmiUiActionModel *actions; UmiUiMenuModel *menus; UmiUiToolbarModel *toolbars; UmiUiStatusModel *status; UmiUiSelectionModel *selection; UmiUiNotificationCentre *notifications; UmiUiDialogService *dialogs; UmiUiContributionModel *contributions; UmiUiBindingRegistry *bindings; UmiUiEventQueue *events; UmiUiViewFactoryRegistry *factories; UmiMutex *mutex; };
static void destroy_all(UmiUiWorkbench *w){if(w==NULL)return;umi_ui_view_factory_registry_destroy(w->factories);umi_ui_event_queue_destroy(w->events);umi_ui_binding_registry_destroy(w->bindings);umi_ui_contribution_model_destroy(w->contributions);umi_ui_dialog_service_destroy(w->dialogs);umi_ui_notification_centre_destroy(w->notifications);umi_ui_selection_destroy(w->selection);umi_ui_status_model_destroy(w->status);umi_ui_toolbar_model_destroy(w->toolbars);umi_ui_menu_model_destroy(w->menus);umi_ui_action_model_destroy(w->actions);umi_ui_layout_destroy(w->layout);umi_ui_perspective_model_destroy(w->perspectives);umi_ui_editor_model_destroy(w->editors);umi_ui_document_view_model_destroy(w->documents);umi_ui_pane_model_destroy(w->panes);umi_mutex_destroy(w->mutex);free(w);}
UmiStatus umi_ui_workbench_create(const char *id,UmiCommandRegistry *commands,UmiUiWorkbench **out){UmiUiWorkbench *w;UmiStatus s;if(out==NULL||commands==NULL||!umi_ui_id_is_valid(id))return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;w=calloc(1U,sizeof(*w));if(w==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)umi_ui_copy_text(w->id,sizeof(w->id),id);w->commands=commands;w->revision=1U;s=umi_mutex_create(&w->mutex);if(s==UMI_STATUS_OK)s=umi_ui_pane_model_create(&w->panes);if(s==UMI_STATUS_OK)s=umi_ui_document_view_model_create(&w->documents);if(s==UMI_STATUS_OK)s=umi_ui_editor_model_create(&w->editors);if(s==UMI_STATUS_OK)s=umi_ui_perspective_model_create(&w->perspectives);if(s==UMI_STATUS_OK)s=umi_ui_layout_create(&w->layout);if(s==UMI_STATUS_OK)s=umi_ui_action_model_create(&w->actions);if(s==UMI_STATUS_OK)s=umi_ui_menu_model_create(&w->menus);if(s==UMI_STATUS_OK)s=umi_ui_toolbar_model_create(&w->toolbars);if(s==UMI_STATUS_OK)s=umi_ui_status_model_create(&w->status);if(s==UMI_STATUS_OK)s=umi_ui_selection_create(&w->selection);if(s==UMI_STATUS_OK)s=umi_ui_notification_centre_create(&w->notifications);if(s==UMI_STATUS_OK)s=umi_ui_dialog_service_create(&w->dialogs);if(s==UMI_STATUS_OK)s=umi_ui_contribution_model_create(&w->contributions);if(s==UMI_STATUS_OK)s=umi_ui_binding_registry_create(&w->bindings);if(s==UMI_STATUS_OK)s=umi_ui_event_queue_create(&w->events);if(s==UMI_STATUS_OK)s=umi_ui_view_factory_registry_create(&w->factories);if(s!=UMI_STATUS_OK){destroy_all(w);return s;}*out=w;return UMI_STATUS_OK;}
void umi_ui_workbench_destroy(UmiUiWorkbench *w){destroy_all(w);}
UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *w,const char *id){UmiUiPerspectiveSnapshot item;size_t i;if(w==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_perspective_model_find(w->perspectives,id,&item)!=UMI_STATUS_OK)return UMI_STATUS_NOT_FOUND;for(i=0U;i<umi_ui_perspective_model_count(w->perspectives);++i){if(umi_ui_perspective_model_at(w->perspectives,i,&item)==UMI_STATUS_OK){item.active=strcmp(item.perspective_id,id)==0;(void)umi_ui_perspective_model_upsert(w->perspectives,&item);}}(void)umi_mutex_lock(w->mutex);(void)umi_ui_copy_text(w->active_perspective,sizeof(w->active_perspective),id);w->revision=umi_ui_next_revision(w->revision);(void)umi_mutex_unlock(w->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *w,const char *id){UmiUiDocumentViewSnapshot item;size_t i;if(w==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_document_view_model_find(w->documents,id,&item)!=UMI_STATUS_OK)return UMI_STATUS_NOT_FOUND;for(i=0U;i<umi_ui_document_view_model_count(w->documents);++i){if(umi_ui_document_view_model_at(w->documents,i,&item)==UMI_STATUS_OK){item.active=strcmp(item.view_id,id)==0;(void)umi_ui_document_view_model_upsert(w->documents,&item);}}(void)umi_mutex_lock(w->mutex);(void)umi_ui_copy_text(w->active_document,sizeof(w->active_document),id);w->revision=umi_ui_next_revision(w->revision);(void)umi_mutex_unlock(w->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *w,const char *action_id,const char *argument,char *out,size_t cap){UmiUiActionSnapshot action;if(w==NULL||action_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_ui_action_model_find(w->actions,action_id,&action)!=UMI_STATUS_OK)return UMI_STATUS_NOT_FOUND;if(!action.enabled||!action.visible)return UMI_STATUS_PERMISSION_DENIED;return umi_command_registry_execute(w->commands,action.command_id,argument,out,cap);}
UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *w,UmiUiWorkbenchSnapshot *out){if(w==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(w->mutex);(void)umi_ui_copy_text(out->workbench_id,sizeof(out->workbench_id),w->id);(void)umi_ui_copy_text(out->active_perspective,sizeof(out->active_perspective),w->active_perspective);(void)umi_ui_copy_text(out->active_document_view,sizeof(out->active_document_view),w->active_document);out->pane_count=umi_ui_pane_model_count(w->panes);out->document_count=umi_ui_document_view_model_count(w->documents);out->perspective_count=umi_ui_perspective_model_count(w->perspectives);out->action_count=umi_ui_action_model_count(w->actions);out->notification_count=umi_ui_notification_count(w->notifications,0);out->revision=w->revision;(void)umi_mutex_unlock(w->mutex);return UMI_STATUS_OK;}
#define ACCESSOR(type,name,field) type *name(UmiUiWorkbench *w){return w!=NULL?w->field:NULL;}
ACCESSOR(UmiUiPaneModel,umi_ui_workbench_panes,panes)
ACCESSOR(UmiUiDocumentViewModel,umi_ui_workbench_documents,documents)
ACCESSOR(UmiUiEditorModel,umi_ui_workbench_editors,editors)
ACCESSOR(UmiUiPerspectiveModel,umi_ui_workbench_perspectives,perspectives)
ACCESSOR(UmiUiLayout,umi_ui_workbench_layout,layout)
ACCESSOR(UmiUiActionModel,umi_ui_workbench_actions,actions)
ACCESSOR(UmiUiMenuModel,umi_ui_workbench_menus,menus)
ACCESSOR(UmiUiToolbarModel,umi_ui_workbench_toolbars,toolbars)
ACCESSOR(UmiUiStatusModel,umi_ui_workbench_status,status)
ACCESSOR(UmiUiSelectionModel,umi_ui_workbench_selection,selection)
ACCESSOR(UmiUiNotificationCentre,umi_ui_workbench_notifications,notifications)
ACCESSOR(UmiUiDialogService,umi_ui_workbench_dialogs,dialogs)
ACCESSOR(UmiUiContributionModel,umi_ui_workbench_contributions,contributions)
ACCESSOR(UmiUiBindingRegistry,umi_ui_workbench_bindings,bindings)
ACCESSOR(UmiUiEventQueue,umi_ui_workbench_events,events)
ACCESSOR(UmiUiViewFactoryRegistry,umi_ui_workbench_view_factories,factories)
#undef ACCESSOR
