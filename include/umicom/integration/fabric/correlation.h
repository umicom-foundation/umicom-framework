/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/correlation.h
 *
 * PURPOSE:
 *   Validate request/reply and causation relationships without coupling callers to a transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CORRELATION_H
#define UMICOM_INTEGRATION_FABRIC_CORRELATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/message_header.h"
#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_fabric_correlation_set_causation(UmiFabricMessageHeader *header,const char *causation_id);
bool umi_fabric_correlation_matches(const UmiFabricMessageHeader *left,const UmiFabricMessageHeader *right);

#ifdef __cplusplus
}
#endif
#endif
