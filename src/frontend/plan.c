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

/*
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/plan.h"
#include "../base/value_construction_internal.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise frontend plan from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_frontend_plan_init(UmiFrontendPlan *p,const char *id,UmiFrontendKind kind){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));p->kind=kind;return umi_web_copy_text(p->application_id,sizeof(p->application_id),id);}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_frontend_plan_init_build_value(UmiFrontendPlan *p,const char *id,UmiFrontendKind kind){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));p->kind=kind;return umi_web_copy_text(p->application_id,sizeof(p->application_id),id);}

UMI_DEFINE_STAGED_VALUE_INIT(umi_frontend_plan_init, UmiFrontendPlan, p,
    (UmiFrontendPlan *p, const char *id, UmiFrontendKind kind),
    umi_frontend_plan_init_build_value(candidate, id, kind))

/*
 * Provide the frontend plan add page operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_page(UmiFrontendPlan *p,const UmiFrontendPage *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->page_count>=UMI_FRONTEND_MAX_PAGES)return UMI_STATUS_CAPACITY_EXCEEDED;p->pages[p->page_count++]=*v;return UMI_STATUS_OK;}
/*
 * Provide the frontend plan add route operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_route(UmiFrontendPlan *p,const UmiFrontendRoute *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->route_count>=UMI_FRONTEND_MAX_ROUTES)return UMI_STATUS_CAPACITY_EXCEEDED;p->routes[p->route_count++]=*v;return UMI_STATUS_OK;}
/*
 * Provide the frontend plan add asset operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_asset(UmiFrontendPlan *p,const UmiFrontendAsset *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||v==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->asset_count>=UMI_FRONTEND_MAX_ASSETS)return UMI_STATUS_CAPACITY_EXCEEDED;p->assets[p->asset_count++]=*v;return UMI_STATUS_OK;}
