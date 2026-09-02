/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/request_deadline.h
 *
 * PURPOSE:
 *   Compute deadline expiry and remaining budget without reading a platform clock directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the fabric request deadline data shared with callers of this public contract.
 */
typedef struct UmiFabricRequestDeadline { uint64_t started_ms; uint64_t deadline_ms; } UmiFabricRequestDeadline;
/**
 * Initialise fabric request deadline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_request_deadline_init(UmiFabricRequestDeadline *deadline,uint64_t now_ms,uint64_t timeout_ms);
/**
 * Provide the fabric request deadline expired operation used by this module and its client
 * applications.
 */
bool umi_fabric_request_deadline_expired(const UmiFabricRequestDeadline *deadline,uint64_t now_ms);
/**
 * Provide the fabric request deadline remaining operation used by this module and its
 * client applications.
 */
uint64_t umi_fabric_request_deadline_remaining(const UmiFabricRequestDeadline *deadline,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
