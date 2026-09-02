/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/frontend.h
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
 * Product APIs describe composition and identity only. Reusable mechanisms stay in Framework while each Umicom application supplies its own domain behaviour.
 */
#ifndef UMICOM_PRODUCT_FRONTEND_H
#define UMICOM_PRODUCT_FRONTEND_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named product frontend kind values accepted by this public contract.
 */
typedef enum UmiProductFrontendKind { UMI_PRODUCT_FRONTEND_CONSOLE=1, UMI_PRODUCT_FRONTEND_GTK4=2, UMI_PRODUCT_FRONTEND_WEB=3, UMI_PRODUCT_FRONTEND_QT=4, UMI_PRODUCT_FRONTEND_WT=5, UMI_PRODUCT_FRONTEND_MOBILE=6 } UmiProductFrontendKind;
/**
 * Represent the product frontend data shared with callers of this public contract.
 */
typedef struct UmiProductFrontend { UmiProductFrontendKind kind; int enabled; } UmiProductFrontend;
/**
 * Provide the product frontend name operation used by this module and its client
 * applications.
 */
const char *umi_product_frontend_name(UmiProductFrontendKind kind);
#ifdef __cplusplus
}
#endif
#endif
