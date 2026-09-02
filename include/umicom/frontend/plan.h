/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/plan.h
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
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_PLAN_H
#define UMICOM_FRONTEND_PLAN_H
#include "umicom/frontend/page.h"
#include "umicom/frontend/route.h"
#include "umicom/frontend/asset.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the frontend plan data shared with callers of this public contract.
 */
typedef struct UmiFrontendPlan { char application_id[UMI_FRONTEND_ID_CAPACITY]; UmiFrontendKind kind; UmiFrontendPage pages[UMI_FRONTEND_MAX_PAGES]; size_t page_count; UmiFrontendRoute routes[UMI_FRONTEND_MAX_ROUTES]; size_t route_count; UmiFrontendAsset assets[UMI_FRONTEND_MAX_ASSETS]; size_t asset_count; } UmiFrontendPlan;
/**
 * Initialise frontend plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_frontend_plan_init(UmiFrontendPlan *plan,const char *application_id,UmiFrontendKind kind);
/**
 * Provide the frontend plan add page operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_page(UmiFrontendPlan *plan,const UmiFrontendPage *page);
/**
 * Provide the frontend plan add route operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_route(UmiFrontendPlan *plan,const UmiFrontendRoute *route);
/**
 * Provide the frontend plan add asset operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_plan_add_asset(UmiFrontendPlan *plan,const UmiFrontendAsset *asset);
#ifdef __cplusplus
}
#endif
#endif
