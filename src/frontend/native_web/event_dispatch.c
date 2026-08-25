/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/event_dispatch.c
 *
 * PURPOSE:
 *   Dispatch semantic UI events through registered Framework callbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/event_dispatch.h"

#include <string.h>
void umi_native_web_event_dispatch_init(UmiNativeWebEventDispatcher *dispatcher){if(dispatcher!=NULL)(void)memset(dispatcher,0,sizeof(*dispatcher));}
UmiStatus umi_native_web_event_dispatch_register(UmiNativeWebEventDispatcher *dispatcher,UmiNativeWebEventKind kind,UmiNativeWebEventHandler handler,void *context){size_t i;if(dispatcher==NULL||handler==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<dispatcher->count;++i)if(dispatcher->entries[i].kind==kind)return UMI_STATUS_ALREADY_EXISTS;if(dispatcher->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;dispatcher->entries[dispatcher->count]=(UmiNativeWebEventDispatchEntry){kind,handler,context};++dispatcher->count;return UMI_STATUS_OK;}
UmiStatus umi_native_web_event_dispatch_one(UmiNativeWebEventDispatcher *dispatcher,const UmiNativeWebUiEvent *event){size_t i;if(dispatcher==NULL||event==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<dispatcher->count;++i)if(dispatcher->entries[i].kind==event->kind)return dispatcher->entries[i].handler(dispatcher->entries[i].context,event);return UMI_STATUS_NOT_FOUND;}

