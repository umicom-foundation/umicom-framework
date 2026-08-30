/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/application_shell.c
 *
 * PURPOSE:
 *   Implement product-neutral application-shell state without owning the workbench lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/application_shell.h"
#include <stdlib.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiApplicationShell { char application_id[UMI_UI_ID_CAPACITY]; char title[UMI_UI_TEXT_CAPACITY]; char status[UMI_UI_DESCRIPTION_CAPACITY]; UmiUiRect bounds; int maximised; int full_screen; int visible; uint64_t revision; UmiUiWorkbench *workbench; UmiMutex *mutex; };
UmiStatus umi_ui_application_shell_create(const char *id,const char *title,UmiUiWorkbench *workbench,UmiUiApplicationShell **out){UmiUiApplicationShell *s;UmiStatus st;if(out==NULL||workbench==NULL||!umi_ui_id_is_valid(id)||title==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=calloc(1U,sizeof(*s));if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)umi_ui_copy_text(s->application_id,sizeof(s->application_id),id);if(!umi_ui_copy_text(s->title,sizeof(s->title),title)){free(s);return UMI_STATUS_CAPACITY_EXCEEDED;}s->workbench=workbench;s->visible=1;s->bounds=(UmiUiRect){0,0,1200,800};s->revision=1U;st=umi_mutex_create(&s->mutex);if(st!=UMI_STATUS_OK){free(s);return st;}*out=s;return UMI_STATUS_OK;}
void umi_ui_application_shell_destroy(UmiUiApplicationShell *s){if(s==NULL)return;umi_mutex_destroy(s->mutex);free(s);}
UmiStatus umi_ui_application_shell_set_title(UmiUiApplicationShell *s,const char *text){if(s==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);if(!umi_ui_copy_text(s->title,sizeof(s->title),text)){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_application_shell_set_status(UmiUiApplicationShell *s,const char *text){if(s==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);if(!umi_ui_copy_text(s->status,sizeof(s->status),text)){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_application_shell_set_bounds(UmiUiApplicationShell *s,UmiUiRect bounds){if(s==NULL||bounds.width<=0||bounds.height<=0)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);s->bounds=bounds;s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_application_shell_set_visible(UmiUiApplicationShell *s,int visible){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);s->visible=visible!=0;s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_application_shell_set_maximised(UmiUiApplicationShell *s,int value){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);s->maximised=value!=0;s->revision=umi_ui_next_revision(s->revision);(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_application_shell_snapshot(const UmiUiApplicationShell *s,UmiUiApplicationShellSnapshot *out){if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);(void)umi_ui_copy_text(out->application_id,sizeof(out->application_id),s->application_id);(void)umi_ui_copy_text(out->title,sizeof(out->title),s->title);(void)umi_ui_copy_text(out->status_text,sizeof(out->status_text),s->status);out->bounds=s->bounds;out->maximised=s->maximised;out->full_screen=s->full_screen;out->visible=s->visible;out->revision=s->revision;(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
UmiUiWorkbench *umi_ui_application_shell_workbench(UmiUiApplicationShell *s){return s!=NULL?s->workbench:NULL;}
