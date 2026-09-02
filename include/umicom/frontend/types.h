/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/types.h
 *
 * PURPOSE:
 *   Define supported frontend kinds and shared capacities.
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

#ifndef UMICOM_FRONTEND_TYPES_H
#define UMICOM_FRONTEND_TYPES_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FRONTEND_ID_CAPACITY 128U
#define UMI_FRONTEND_TEXT_CAPACITY 512U
#define UMI_FRONTEND_MAX_ROUTES 128U
#define UMI_FRONTEND_MAX_ASSETS 256U
#define UMI_FRONTEND_MAX_PAGES 128U
/**
 * List the named frontend kind values accepted by this public contract.
 */
typedef enum UmiFrontendKind { UMI_FRONTEND_KIND_HEADLESS=1, UMI_FRONTEND_KIND_GTK4=2, UMI_FRONTEND_KIND_WEB=3, UMI_FRONTEND_KIND_QT=4, UMI_FRONTEND_KIND_WT=5, UMI_FRONTEND_KIND_MOBILE=6 } UmiFrontendKind;
/**
 * Provide the frontend kind text operation used by this module and its client
 * applications.
 */
const char *umi_frontend_kind_text(UmiFrontendKind kind);
#ifdef __cplusplus
}
#endif
#endif
