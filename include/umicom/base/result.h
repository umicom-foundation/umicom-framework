/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/result.h
 *
 * PURPOSE:
 *   Publish the public result contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_RESULT_H
#define UMICOM_BASE_RESULT_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the result data shared with callers of this public contract.
 */
typedef struct UmiResult {
    UmiStatus status;
    const char *message;
} UmiResult;

/**
 * Provide the result ok operation used by this module and its client applications.
 */
UmiResult umi_result_ok(void);
/**
 * Provide the result error operation used by this module and its client applications.
 */
UmiResult umi_result_error(UmiStatus status, const char *message);
/**
 * Provide the result is ok operation used by this module and its client applications.
 */
int umi_result_is_ok(UmiResult result);

#ifdef __cplusplus
}
#endif

#endif
