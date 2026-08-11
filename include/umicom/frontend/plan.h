/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/plan.h
 *
 * PURPOSE:
 *   Collect pages, routes and assets into one frontend application plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
typedef struct UmiFrontendPlan { char application_id[UMI_FRONTEND_ID_CAPACITY]; UmiFrontendKind kind; UmiFrontendPage pages[UMI_FRONTEND_MAX_PAGES]; size_t page_count; UmiFrontendRoute routes[UMI_FRONTEND_MAX_ROUTES]; size_t route_count; UmiFrontendAsset assets[UMI_FRONTEND_MAX_ASSETS]; size_t asset_count; } UmiFrontendPlan;
UmiStatus umi_frontend_plan_init(UmiFrontendPlan *plan,const char *application_id,UmiFrontendKind kind);
UmiStatus umi_frontend_plan_add_page(UmiFrontendPlan *plan,const UmiFrontendPage *page);
UmiStatus umi_frontend_plan_add_route(UmiFrontendPlan *plan,const UmiFrontendRoute *route);
UmiStatus umi_frontend_plan_add_asset(UmiFrontendPlan *plan,const UmiFrontendAsset *asset);
#ifdef __cplusplus
}
#endif
#endif
