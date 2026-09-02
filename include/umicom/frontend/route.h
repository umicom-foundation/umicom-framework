/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/route.h
 *
 * PURPOSE:
 *   Map a URL path to a semantic page.
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

#ifndef UMICOM_FRONTEND_ROUTE_H
#define UMICOM_FRONTEND_ROUTE_H
#include "umicom/frontend/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the frontend route data shared with callers of this public contract.
 */
typedef struct UmiFrontendRoute { char path[UMI_FRONTEND_TEXT_CAPACITY]; char page_id[UMI_FRONTEND_ID_CAPACITY]; } UmiFrontendRoute;
/**
 * Initialise frontend route from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_route_init(UmiFrontendRoute *route,const char *path,const char *page_id);
#ifdef __cplusplus
}
#endif
#endif
