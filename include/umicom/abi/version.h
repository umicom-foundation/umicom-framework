/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/version.h
 *
 * PURPOSE:
 *   Compare semantic and ABI versions without exposing compiler-specific types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_VERSION_H
#define UMICOM_ABI_VERSION_H
#include "umicom/base/version.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_abi_version_compare(UmiVersion left, UmiVersion right);
int umi_abi_version_is_compatible(UmiVersion required, UmiVersion available);
#ifdef __cplusplus
}
#endif
#endif
