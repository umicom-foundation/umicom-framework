/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/feature.h
 *
 * PURPOSE:
 *   Describe one ABI feature and its maturity or optionality.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_FEATURE_H
#define UMICOM_ABI_FEATURE_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiFeature { const char *feature_id; uint32_t version; int required; } UmiAbiFeature;
UmiStatus umi_abi_feature_validate(const UmiAbiFeature *feature);
#ifdef __cplusplus
}
#endif
#endif
