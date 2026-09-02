/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/generator.h
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

/*
 * A frontend plan describes application meaning. It does not make GTK, React, Qt or Wt the owner of application state.
 */

#ifndef UMICOM_FRONTEND_GENERATOR_H
#define UMICOM_FRONTEND_GENERATOR_H
#include "umicom/frontend/plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the frontend generate html operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_generate_html(const UmiFrontendPlan *plan,char *out_text,size_t capacity);
/**
 * Provide the frontend generate manifest operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_generate_manifest(const UmiFrontendPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
