/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/route_matcher.h
 *
 * PURPOSE:
 *   Match exact or suffix-wildcard patterns and choose the highest-priority enabled route.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ROUTE_MATCHER_H
#define UMICOM_INTEGRATION_FABRIC_ROUTE_MATCHER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/route_table.h"
#ifdef __cplusplus
extern "C" {
#endif


bool umi_fabric_route_pattern_match(const char *pattern,const char *value);
UmiStatus umi_fabric_route_match(const UmiFabricRouteTable *table,const char *source,const char *message_type,UmiFabricRouteRule *out_rule);

#ifdef __cplusplus
}
#endif
#endif
