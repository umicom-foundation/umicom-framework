/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/locator.h
 *
 * PURPOSE:
 *   Locate an installed SDK from an explicit prefix or conventional directory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_LOCATOR_H
#define UMICOM_SDK_LOCATOR_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_sdk_locate(const char *preferred_prefix,char *out_prefix,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
