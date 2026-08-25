/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/request_deadline.c
 *
 * PURPOSE:
 *   Compute deadline expiry and remaining budget without reading a platform clock directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/request_deadline.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_request_deadline_init(UmiFabricRequestDeadline *deadline,uint64_t now_ms,uint64_t timeout_ms){if(deadline==NULL||timeout_ms==0U||UINT64_MAX-now_ms<timeout_ms)return UMI_STATUS_INVALID_ARGUMENT;deadline->started_ms=now_ms;deadline->deadline_ms=now_ms+timeout_ms;return UMI_STATUS_OK;}
bool umi_fabric_request_deadline_expired(const UmiFabricRequestDeadline *deadline,uint64_t now_ms){return deadline==NULL||now_ms>=deadline->deadline_ms;}
uint64_t umi_fabric_request_deadline_remaining(const UmiFabricRequestDeadline *deadline,uint64_t now_ms){if(deadline==NULL||now_ms>=deadline->deadline_ms)return 0U;return deadline->deadline_ms-now_ms;}
