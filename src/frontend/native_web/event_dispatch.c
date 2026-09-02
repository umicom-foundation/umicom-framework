/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/event_dispatch.c
 *
 * PURPOSE:
 *   Dispatch semantic UI events through registered Framework callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/event_dispatch.h"

#include <string.h>
/*
 * Initialise native web event dispatch from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_event_dispatch_init(UmiNativeWebEventDispatcher *dispatcher){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher!=NULL)(void)memset(dispatcher,0,sizeof(*dispatcher));}
/*
 * Add native web event dispatch only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_native_web_event_dispatch_register(UmiNativeWebEventDispatcher *dispatcher,UmiNativeWebEventKind kind,UmiNativeWebEventHandler handler,void *context){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher==NULL||handler==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<dispatcher->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher->entries[i].kind==kind)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;dispatcher->entries[dispatcher->count]=(UmiNativeWebEventDispatchEntry){kind,handler,context};++dispatcher->count;return UMI_STATUS_OK;}
/*
 * Provide the native web event dispatch one operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_event_dispatch_one(UmiNativeWebEventDispatcher *dispatcher,const UmiNativeWebUiEvent *event){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher==NULL||event==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<dispatcher->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dispatcher->entries[i].kind==event->kind)return dispatcher->entries[i].handler(dispatcher->entries[i].context,event);return UMI_STATUS_NOT_FOUND;}

