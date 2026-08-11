/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/rest.c
 *
 * PURPOSE:
 *   Implement REST registration and JSON response helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/rest.h"
UmiStatus umi_web_rest_register(UmiWebRouter *router,UmiHttpMethod method,const char *path,UmiWebHandler handler,void *user_data){UmiWebRoute route;UmiStatus s=umi_web_route_init(&route,method,path,handler,user_data);return s==UMI_STATUS_OK?umi_web_router_add(router,&route):s;}
UmiStatus umi_web_rest_json(UmiWebResponse *response,int status,const char *json){return umi_web_response_set_text(response,status,"application/json; charset=utf-8",json);}
