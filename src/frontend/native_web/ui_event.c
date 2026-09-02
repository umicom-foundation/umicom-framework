/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/ui_event.c
 *
 * PURPOSE:
 *   Define browser-originated semantic UI events with no direct DOM ownership in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/ui_event.h"

#include <string.h>
/*
 * Initialise native web ui event from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_native_web_ui_event_init(UmiNativeWebUiEvent *event,UmiNativeWebEventKind kind,const char *target_id,const char *name,const char *value,uint64_t sequence){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(event==NULL||target_id==NULL||target_id[0]=='\0'||kind<UMI_NATIVE_WEB_EVENT_CLICK||kind>UMI_NATIVE_WEB_EVENT_CONTEXT)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(event,0,sizeof(*event));event->kind=kind;s=umi_native_web_copy_text(event->target_id,sizeof(event->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(name!=NULL){s=umi_native_web_copy_text(event->name,sizeof(event->name),name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL){s=umi_native_web_copy_text(event->value,sizeof(event->value),value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}event->sequence=sequence;return UMI_STATUS_OK;}

