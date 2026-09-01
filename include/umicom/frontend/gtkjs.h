/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/gtkjs.h
 *
 * PURPOSE:
 *   Generate a gtk-js/React-oriented frontend project from the shared plan.
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

#ifndef UMICOM_FRONTEND_GTKJS_H
#define UMICOM_FRONTEND_GTKJS_H
#include "umicom/frontend/renderer.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_frontend_gtkjs_create(UmiFrontendRenderer *out_renderer);
#ifdef __cplusplus
}
#endif
#endif
