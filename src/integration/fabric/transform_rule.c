/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/transform_rule.c
 *
 * PURPOSE:
 *   Describe a deterministic field/content transform without embedding a scripting runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/transform_rule.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric transform rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_transform_rule_init(UmiFabricTransformRule *item, const char *rule_id, const char *source_path, const char *target_path, const char *operation, bool required) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->rule_id,sizeof(item->rule_id),rule_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->source_path,sizeof(item->source_path),source_path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->target_path,sizeof(item->target_path),target_path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->required=required;
    return umi_fabric_transform_rule_validate(item);
}
/*
 * Check that fabric transform rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_transform_rule_validate(const UmiFabricTransformRule *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->rule_id[0]!='\0' && item->source_path[0]!='\0' && item->target_path[0]!='\0' && item->operation[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
