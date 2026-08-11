/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/renderer.h
 *
 * PURPOSE:
 *   Define a stable frontend-renderer function table.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_RENDERER_H
#define UMICOM_FRONTEND_RENDERER_H
#include "umicom/frontend/plan.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFrontendRendererV1 { void *instance; UmiFrontendKind kind; UmiStatus (*render)(void *instance,const UmiFrontendPlan *plan,const char *output_root); void (*destroy)(void *instance); } UmiFrontendRendererV1;
UmiStatus umi_frontend_renderer_validate(const UmiFrontendRendererV1 *renderer);
#ifdef __cplusplus
}
#endif
#endif
