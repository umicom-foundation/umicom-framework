/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/launcher.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral launcher decision for console and graphical entry points.
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
#ifndef UMICOM_PRODUCT_LAUNCHER_H
#define UMICOM_PRODUCT_LAUNCHER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/product/profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named product launch mode values accepted by this public contract.
 */
typedef enum UmiProductLaunchMode { UMI_PRODUCT_LAUNCH_CONSOLE=1, UMI_PRODUCT_LAUNCH_GUI=2, UMI_PRODUCT_LAUNCH_WEB=3 } UmiProductLaunchMode;
/**
 * Provide the product launcher select operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_launcher_select(const UmiProductProfile *profile,UmiProductLaunchMode preferred,UmiProductFrontendKind *out_frontend);
#ifdef __cplusplus
}
#endif
#endif
