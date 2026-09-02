/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/renderer.h
 *
 * PURPOSE:
 *   Define a stable frontend-renderer function table.
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

#ifndef UMICOM_FRONTEND_RENDERER_H
#define UMICOM_FRONTEND_RENDERER_H
#include "umicom/frontend/plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the frontend renderer data shared with callers of this public contract.
 */
typedef struct UmiFrontendRenderer { void *instance; UmiFrontendKind kind; UmiStatus (*render)(void *instance,const UmiFrontendPlan *plan,const char *output_root); void (*destroy)(void *instance); } UmiFrontendRenderer;
/**
 * Check that frontend renderer satisfies its contract before another service relies on it.
 */
UmiStatus umi_frontend_renderer_validate(const UmiFrontendRenderer *renderer);
#ifdef __cplusplus
}
#endif
#endif
