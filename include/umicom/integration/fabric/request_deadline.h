/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/request_deadline.h
 *
 * PURPOSE:
 *   Compute deadline expiry and remaining budget without reading a platform clock directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_REQUEST_DEADLINE_H
#define UMICOM_INTEGRATION_FABRIC_REQUEST_DEADLINE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricRequestDeadline { uint64_t started_ms; uint64_t deadline_ms; } UmiFabricRequestDeadline;
UmiStatus umi_fabric_request_deadline_init(UmiFabricRequestDeadline *deadline,uint64_t now_ms,uint64_t timeout_ms);
bool umi_fabric_request_deadline_expired(const UmiFabricRequestDeadline *deadline,uint64_t now_ms);
uint64_t umi_fabric_request_deadline_remaining(const UmiFabricRequestDeadline *deadline,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
