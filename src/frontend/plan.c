/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/plan.c
 *
 * PURPOSE:
 *   Collect pages, routes and assets into one frontend application plan.
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

#include "umicom/frontend/plan.h"
#include "umicom/web/types.h"
#include <string.h>
UmiStatus umi_frontend_plan_init(UmiFrontendPlan *p,const char *id,UmiFrontendKind kind){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));p->kind=kind;return umi_web_copy_text(p->application_id,sizeof(p->application_id),id);}
UmiStatus umi_frontend_plan_add_page(UmiFrontendPlan *p,const UmiFrontendPage *v){if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(p->page_count>=UMI_FRONTEND_MAX_PAGES)return UMI_STATUS_CAPACITY_EXCEEDED;p->pages[p->page_count++]=*v;return UMI_STATUS_OK;}
UmiStatus umi_frontend_plan_add_route(UmiFrontendPlan *p,const UmiFrontendRoute *v){if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(p->route_count>=UMI_FRONTEND_MAX_ROUTES)return UMI_STATUS_CAPACITY_EXCEEDED;p->routes[p->route_count++]=*v;return UMI_STATUS_OK;}
UmiStatus umi_frontend_plan_add_asset(UmiFrontendPlan *p,const UmiFrontendAsset *v){if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(p->asset_count>=UMI_FRONTEND_MAX_ASSETS)return UMI_STATUS_CAPACITY_EXCEEDED;p->assets[p->asset_count++]=*v;return UMI_STATUS_OK;}
