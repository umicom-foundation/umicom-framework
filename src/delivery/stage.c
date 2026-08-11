/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/stage.c
 *
 * PURPOSE:
 *   Track the installation-staging directory that will become an installable or portable package.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Staging separates build outputs from the exact files that users will receive.
 */

#include "umicom/delivery/stage.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_delivery_stage_init(UmiDeliveryStageRecord *stage,
                                  const char *root)
{
    if (stage == NULL || root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(stage, 0, sizeof(*stage));
    return umi_delivery_copy_text(stage->root, sizeof(stage->root), root);
}

void umi_delivery_stage_add_file(UmiDeliveryStageRecord *stage,
                                 uint64_t size_bytes)
{
    if (stage != NULL) {
        ++stage->file_count;
        stage->total_bytes += size_bytes;
    }
}

void umi_delivery_stage_complete(UmiDeliveryStageRecord *stage)
{
    if (stage != NULL) stage->complete = 1;
}
