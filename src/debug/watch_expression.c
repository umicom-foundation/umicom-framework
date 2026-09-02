/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/watch_expression.c
 *
 * PURPOSE:
 *   Implement the watch expression behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework debugger watch expressions. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/watch_expression.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the debug watch evaluation from snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_watch_evaluation_from_snapshot(const UmiDebugWatchSnapshot*w,int frame,UmiDebugWatchEvaluation*out){int a,b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||out==NULL||w->id[0]=='\0'||w->expression[0]=='\0'||frame<0)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));a=snprintf(out->watch_id,sizeof(out->watch_id),"%s",w->id);b=snprintf(out->expression,sizeof(out->expression),"%s",w->expression);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a<0||b<0||(size_t)a>=sizeof(out->watch_id)||(size_t)b>=sizeof(out->expression))return UMI_STATUS_CAPACITY_EXCEEDED;(void)snprintf(out->context,sizeof(out->context),"watch");out->frame_id=frame;out->enabled=w->enabled;out->revision=w->revision;return UMI_STATUS_OK;}
