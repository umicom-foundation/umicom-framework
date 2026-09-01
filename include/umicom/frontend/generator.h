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
UmiStatus umi_frontend_generate_html(const UmiFrontendPlan *plan,char *out_text,size_t capacity);
UmiStatus umi_frontend_generate_manifest(const UmiFrontendPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
