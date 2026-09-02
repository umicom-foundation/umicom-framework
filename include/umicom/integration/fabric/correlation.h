/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/correlation.h
 *
 * PURPOSE:
 *   Validate request/reply and causation relationships without coupling callers to a transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Provide the fabric correlation set causation operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_correlation_set_causation(UmiFabricMessageHeader *header,const char *causation_id);
/**
 * Provide the fabric correlation matches operation used by this module and its client
 * applications.
 */
bool umi_fabric_correlation_matches(const UmiFabricMessageHeader *left,const UmiFabricMessageHeader *right);

#ifdef __cplusplus
}
#endif
#endif
