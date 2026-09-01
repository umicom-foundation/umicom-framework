/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/frontend.c
 *
 * PURPOSE:
 *   Describe the frontend types enabled by one product profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/frontend.h"
const char *umi_product_frontend_name(UmiProductFrontendKind k){switch(k){case UMI_PRODUCT_FRONTEND_CONSOLE:return "console";case UMI_PRODUCT_FRONTEND_GTK4:return "gtk4";case UMI_PRODUCT_FRONTEND_WEB:return "web";case UMI_PRODUCT_FRONTEND_QT:return "qt";case UMI_PRODUCT_FRONTEND_WT:return "wt";case UMI_PRODUCT_FRONTEND_MOBILE:return "mobile";default:return "unknown";}}
