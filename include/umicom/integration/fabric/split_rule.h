/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/split_rule.h
 *
 * PURPOSE:
 *   Describe splitter cardinality and empty-part handling independently of payload parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SPLIT_RULE_H
#define UMICOM_INTEGRATION_FABRIC_SPLIT_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricSplitRule {
    char rule_id[UMI_FABRIC_ID_CAPACITY];
    char expression[UMI_FABRIC_TEXT_CAPACITY];
    size_t maximum_parts;
    bool discard_empty;
} UmiFabricSplitRule;

UmiStatus umi_fabric_split_rule_init(UmiFabricSplitRule *item, const char *rule_id, const char *expression, size_t maximum_parts, bool discard_empty);
UmiStatus umi_fabric_split_rule_validate(const UmiFabricSplitRule *item);

#ifdef __cplusplus
}
#endif
#endif
