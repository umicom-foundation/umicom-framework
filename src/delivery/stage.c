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
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery stage from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_delivery_stage_init_build_value(UmiDeliveryStageRecord *stage,
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_delivery_stage_init, UmiDeliveryStageRecord, stage,
    (UmiDeliveryStageRecord *stage, const char *root),
    umi_delivery_stage_init_build_value(candidate, root))


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
