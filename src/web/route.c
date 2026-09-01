/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/route.c
 *
 * PURPOSE:
 *   Implement exact method/path route matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/route.h"
#include <string.h>
UmiStatus umi_web_route_init(UmiWebRoute *route,UmiHttpMethod method,const char *pattern,UmiWebHandler handler,void *user_data){UmiStatus s;if(route==NULL||pattern==NULL||handler==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(route,0,sizeof(*route));route->method=method;route->handler=handler;route->user_data=user_data;s=umi_web_copy_text(route->pattern,sizeof(route->pattern),pattern);return s;}
int umi_web_route_matches(const UmiWebRoute *route,const UmiWebRequest *request){return route!=NULL&&request!=NULL&&route->method==request->method&&strcmp(route->pattern,request->path)==0;}
