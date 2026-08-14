/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/headless.h
 *
 * PURPOSE:
 *   Provide a deterministic frontend renderer for tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_HEADLESS_H
#define UMICOM_FRONTEND_HEADLESS_H
#include "umicom/frontend/renderer.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_frontend_headless_create(char *capture,size_t capacity,UmiFrontendRenderer *out_renderer);
#ifdef __cplusplus
}
#endif
#endif
