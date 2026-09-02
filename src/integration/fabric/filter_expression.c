/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/filter_expression.c
 *
 * PURPOSE:
 *   Describe a bounded textual equality/prefix filter used by route and workflow policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/filter_expression.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric filter expression from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_filter_expression_init(UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(item->value,sizeof(item->value),value);
    return umi_fabric_filter_expression_validate(item);
}
/*
 * Check that fabric filter expression satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_filter_expression_validate(const UmiFabricFilterExpression *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->field[0]!='\0' && (strcmp(item->operation,"eq")==0 || strcmp(item->operation,"prefix")==0) && item->value[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
