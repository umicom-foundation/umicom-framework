/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/generator.c
 *
 * PURPOSE:
 *   Generate simple browser artifacts and manifests from a frontend plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/generator.h"
#include "umicom/web/json.h"
#include <stdio.h>
UmiStatus umi_frontend_generate_html(const UmiFrontendPlan *p,char *out,size_t cap){int n;if(p==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,cap,"<!doctype html><html><head><meta charset=\"utf-8\"><title>%s</title></head><body><main id=\"umicom-app\" data-app=\"%s\"></main></body></html>",p->application_id,p->application_id);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
UmiStatus umi_frontend_generate_manifest(const UmiFrontendPlan *p,char *out,size_t cap){char id[256];int n;if(p==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_web_json_escape(p->application_id,id,sizeof(id))!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;n=snprintf(out,cap,"{\"application\":\"%s\",\"frontend\":\"%s\",\"pages\":%zu,\"routes\":%zu,\"assets\":%zu}",id,umi_frontend_kind_text(p->kind),p->page_count,p->route_count,p->asset_count);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
