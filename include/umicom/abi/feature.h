/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/feature.h
 *
 * PURPOSE:
 *   Describe one ABI feature and its maturity or optionality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_FEATURE_H
#define UMICOM_ABI_FEATURE_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi feature data shared with callers of this public contract.
 */
typedef struct UmiAbiFeature { const char *feature_id; uint32_t version; int required; } UmiAbiFeature;
/**
 * Check that abi feature satisfies its contract before another service relies on it.
 */
UmiStatus umi_abi_feature_validate(const UmiAbiFeature *feature);
#ifdef __cplusplus
}
#endif
#endif
