/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/request_deadline.c
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
#include "umicom/integration/fabric/request_deadline.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric request deadline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_request_deadline_init(UmiFabricRequestDeadline *deadline,uint64_t now_ms,uint64_t timeout_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(deadline==NULL||timeout_ms==0U||UINT64_MAX-now_ms<timeout_ms)return UMI_STATUS_INVALID_ARGUMENT;deadline->started_ms=now_ms;deadline->deadline_ms=now_ms+timeout_ms;return UMI_STATUS_OK;}
/*
 * Provide the fabric request deadline expired operation used by this module and its client
 * applications.
 */
bool umi_fabric_request_deadline_expired(const UmiFabricRequestDeadline *deadline,uint64_t now_ms){return deadline==NULL||now_ms>=deadline->deadline_ms;}
/*
 * Provide the fabric request deadline remaining operation used by this module and its
 * client applications.
 */
uint64_t umi_fabric_request_deadline_remaining(const UmiFabricRequestDeadline *deadline,uint64_t now_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(deadline==NULL||now_ms>=deadline->deadline_ms)return 0U;return deadline->deadline_ms-now_ms;}
