/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/request_retry.h
 *
 * PURPOSE:
 *   Calculate bounded exponential retry delays while leaving execution to existing resilience services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_REQUEST_RETRY_H
#define UMICOM_INTEGRATION_FABRIC_REQUEST_RETRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricRequestRetry { uint32_t max_attempts; uint64_t initial_delay_ms; uint64_t maximum_delay_ms; double multiplier; } UmiFabricRequestRetry;
void umi_fabric_request_retry_default(UmiFabricRequestRetry *policy);
UmiStatus umi_fabric_request_retry_delay(const UmiFabricRequestRetry *policy,uint32_t attempt,uint64_t *out_delay_ms);

#ifdef __cplusplus
}
#endif
#endif
