/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/dataset.h
 *
 * PURPOSE:
 *   Track a bounded evaluation dataset by stable item identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Datasets record evidence inputs explicitly so quality measurements can be repeated after a provider or prompt changes.
 */

#ifndef INCLUDE_UMICOM_AI_DATASET_H
#define INCLUDE_UMICOM_AI_DATASET_H

#include <stddef.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DATASET_CAPACITY 64U
typedef struct UmiAiDataset {
    char dataset_id[UMI_AI_ID_CAPACITY];
    char item_ids[UMI_AI_DATASET_CAPACITY][UMI_AI_ID_CAPACITY];
    size_t count;
} UmiAiDataset;

void umi_ai_dataset_init(UmiAiDataset *dataset, const char *dataset_id);
UmiStatus umi_ai_dataset_add(UmiAiDataset *dataset, const char *item_id);

#ifdef __cplusplus
}
#endif

#endif
