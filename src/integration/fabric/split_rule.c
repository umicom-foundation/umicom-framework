/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/split_rule.c
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
#include "umicom/integration/fabric/split_rule.h"
#include <string.h>
#include <limits.h>

UmiStatus umi_fabric_split_rule_init(UmiFabricSplitRule *item, const char *rule_id, const char *expression, size_t maximum_parts, bool discard_empty) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->expression,sizeof(item->expression),expression);if(s!=UMI_STATUS_OK)return s;item->maximum_parts=maximum_parts;item->discard_empty=discard_empty;
    return umi_fabric_split_rule_validate(item);
}
UmiStatus umi_fabric_split_rule_validate(const UmiFabricSplitRule *item) {
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->rule_id[0]!='\0' && item->expression[0]!='\0' && item->maximum_parts>0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
