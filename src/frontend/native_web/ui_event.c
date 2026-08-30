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
UmiStatus umi_native_web_ui_event_init(UmiNativeWebUiEvent *event,UmiNativeWebEventKind kind,const char *target_id,const char *name,const char *value,uint64_t sequence){UmiStatus s;if(event==NULL||target_id==NULL||target_id[0]=='\0'||kind<UMI_NATIVE_WEB_EVENT_CLICK||kind>UMI_NATIVE_WEB_EVENT_CONTEXT)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(event,0,sizeof(*event));event->kind=kind;s=umi_native_web_copy_text(event->target_id,sizeof(event->target_id),target_id);if(s!=UMI_STATUS_OK)return s;if(name!=NULL){s=umi_native_web_copy_text(event->name,sizeof(event->name),name);if(s!=UMI_STATUS_OK)return s;}if(value!=NULL){s=umi_native_web_copy_text(event->value,sizeof(event->value),value);if(s!=UMI_STATUS_OK)return s;}event->sequence=sequence;return UMI_STATUS_OK;}

