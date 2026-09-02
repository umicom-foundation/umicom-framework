/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/stage.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_STAGE_H
#define INCLUDE_UMICOM_DELIVERY_STAGE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery stage record data shared with callers of this public contract.
 */
typedef struct UmiDeliveryStageRecord {
    char root[UMI_DELIVERY_PATH_CAPACITY];
    size_t file_count;
    uint64_t total_bytes;
    int complete;
} UmiDeliveryStageRecord;

/**
 * Initialise delivery stage from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_stage_init(UmiDeliveryStageRecord *stage,
                                  const char *root);
/**
 * Provide the delivery stage add file operation used by this module and its client
 * applications.
 */
void umi_delivery_stage_add_file(UmiDeliveryStageRecord *stage,
                                 uint64_t size_bytes);
/**
 * Provide the delivery stage complete operation used by this module and its client
 * applications.
 */
void umi_delivery_stage_complete(UmiDeliveryStageRecord *stage);

#ifdef __cplusplus
}
#endif

#endif
