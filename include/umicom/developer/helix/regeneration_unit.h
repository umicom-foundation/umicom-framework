/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/regeneration_unit.h
 *
 * PURPOSE:
 *   Represent a regenerable module/file unit with baseline, desired and generated fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REGENERATION_UNIT_H
#define UMICOM_DEVELOPER_HELIX_REGENERATION_UNIT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRegenerationUnit {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixRegenerationUnit;
/* Initialise a deterministic, empty regeneration unit record. */
void umi_helix_regeneration_unit_init(UmiHelixRegenerationUnit *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_regeneration_unit_validate(const UmiHelixRegenerationUnit *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_regeneration_unit_score(const UmiHelixRegenerationUnit *record);
#ifdef __cplusplus
}
#endif
#endif
