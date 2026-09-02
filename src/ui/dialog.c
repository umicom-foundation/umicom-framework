/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/dialog.c
 *
 * PURPOSE:
 *   Implement a bounded dialog request queue and result repository for asynchronous adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/dialog.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiDialogService { UmiUiDialogRequest pending[UMI_UI_DIALOG_QUEUE_MAX]; size_t pending_count; UmiUiDialogResult results[UMI_UI_DIALOG_QUEUE_MAX]; size_t result_count; uint64_t next_id; UmiMutex *mutex; };
/*
 * Initialise ui dialog service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_dialog_service_create(UmiUiDialogService **out){UmiUiDialogService *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;st=umi_mutex_create(&s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK){free(s);return st;}s->next_id=1U;*out=s;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui dialog service so the same storage can be reused
 * safely.
 */
void umi_ui_dialog_service_destroy(UmiUiDialogService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;umi_mutex_destroy(s->mutex);free(s);}
/* Provide the ui dialog request operation used by this module and its client applications. */
UmiStatus umi_ui_dialog_request(UmiUiDialogService *s,const UmiUiDialogRequest *r,uint64_t *out_id){UmiUiDialogRequest v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL||r->title[0]=='\0'||r->button_count>UMI_UI_DIALOG_BUTTON_MAX)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->pending_count>=UMI_UI_DIALOG_QUEUE_MAX){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}v=*r;v.request_id=s->next_id++;s->pending[s->pending_count++]=v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_id!=NULL)*out_id=v.request_id;(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/* Provide the ui dialog next operation used by this module and its client applications. */
UmiStatus umi_ui_dialog_next(UmiUiDialogService *s,UmiUiDialogRequest *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->pending_count==0U){(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_NOT_FOUND;}*out=s->pending[0];(void)memmove(&s->pending[0],&s->pending[1],(s->pending_count-1U)*sizeof(s->pending[0]));s->pending_count--; (void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui dialog complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_dialog_complete(UmiUiDialogService *s,const UmiUiDialogResult *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL||r->request_id==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->result_count>=UMI_UI_DIALOG_QUEUE_MAX){(void)memmove(&s->results[0],&s->results[1],(UMI_UI_DIALOG_QUEUE_MAX-1U)*sizeof(s->results[0]));s->result_count=UMI_UI_DIALOG_QUEUE_MAX-1U;}s->results[s->result_count++]=*r;(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}
/* Provide the ui dialog result operation used by this module and its client applications. */
UmiStatus umi_ui_dialog_result(const UmiUiDialogService *s,uint64_t id,UmiUiDialogResult *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(s->mutex);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->result_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->results[i].request_id==id){*out=s->results[i];(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_OK;}(void)umi_mutex_unlock(s->mutex);return UMI_STATUS_NOT_FOUND;}
/*
 * Return the number of records represented by ui dialog pending without changing their
 * state.
 */
size_t umi_ui_dialog_pending_count(const UmiUiDialogService *s){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){(void)umi_mutex_lock(s->mutex);n=s->pending_count;(void)umi_mutex_unlock(s->mutex);}return n;}
