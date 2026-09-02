/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/stage.c
 *
 * PURPOSE:
 *   Track the installation-staging directory that will become an installable or portable package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Staging separates build outputs from the exact files that users will receive.
 */

#include "umicom/delivery/stage.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery stage from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_stage_init(UmiDeliveryStageRecord *stage,
                                  const char *root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stage == NULL || root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(stage, 0, sizeof(*stage));
    return umi_delivery_copy_text(stage->root, sizeof(stage->root), root);
}

/*
 * Provide the delivery stage add file operation used by this module and its client
 * applications.
 */
void umi_delivery_stage_add_file(UmiDeliveryStageRecord *stage,
                                 uint64_t size_bytes)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stage != NULL) {
        ++stage->file_count;
        stage->total_bytes += size_bytes;
    }
}

/*
 * Provide the delivery stage complete operation used by this module and its client
 * applications.
 */
void umi_delivery_stage_complete(UmiDeliveryStageRecord *stage)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stage != NULL) stage->complete = 1;
}
