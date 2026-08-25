/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/repair_strategy.h
 *
 * PURPOSE:
 *   Describe a bounded repair approach and the failure classes for which it may be considered.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REPAIR_STRATEGY_H
#define UMICOM_DEVELOPER_HELIX_REPAIR_STRATEGY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRepairStrategy {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixRepairStrategy;
/* Initialise a deterministic, empty repair strategy record. */
void umi_helix_repair_strategy_init(UmiHelixRepairStrategy *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_repair_strategy_validate(const UmiHelixRepairStrategy *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_repair_strategy_score(const UmiHelixRepairStrategy *record);
#ifdef __cplusplus
}
#endif
#endif
