/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/filter_rule.h
 *
 * PURPOSE:
 *   Evaluate one simple filter against caller-supplied field/value evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_FILTER_RULE_H
#define UMICOM_INTEGRATION_FABRIC_FILTER_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/filter_expression.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the fabric filter rule match operation used by this module and its client
 * applications.
 */
bool umi_fabric_filter_rule_match(const UmiFabricFilterExpression *expression,const char *field,const char *value);

#ifdef __cplusplus
}
#endif
#endif
