/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/types.c
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
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/types.h"
const char *umi_frontend_kind_text(UmiFrontendKind k){switch(k){case UMI_FRONTEND_KIND_HEADLESS:return "headless";case UMI_FRONTEND_KIND_GTK4:return "gtk4";case UMI_FRONTEND_KIND_WEB:return "web";case UMI_FRONTEND_KIND_QT:return "qt";case UMI_FRONTEND_KIND_WT:return "wt";case UMI_FRONTEND_KIND_MOBILE:return "mobile";default:return "unknown";}}
