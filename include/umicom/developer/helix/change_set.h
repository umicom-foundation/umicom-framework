/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/change_set.h
 *
 * PURPOSE:
 *   Collect file changes and compute deterministic aggregate change risk and fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_CHANGE_SET_H
#define UMICOM_DEVELOPER_HELIX_CHANGE_SET_H
#include "umicom/developer/helix/file_change.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixChangeSet { UmiHelixFileChange items[UMI_HELIX_MAX_ITEMS]; size_t count; size_t destructive_count; uint64_t fingerprint; } UmiHelixChangeSet;
/* Initialise an empty change set. */
void umi_helix_change_set_init(UmiHelixChangeSet *set);
/* Add a validated file change and update aggregate destructive/fingerprint evidence. */
UmiStatus umi_helix_change_set_add(UmiHelixChangeSet *set,const UmiHelixFileChange *change,UmiHelixChangeKind kind,const char *path);
/* Compute conservative aggregate risk including destructive-change pressure. */
double umi_helix_change_set_risk(const UmiHelixChangeSet *set);
#ifdef __cplusplus
}
#endif
#endif
