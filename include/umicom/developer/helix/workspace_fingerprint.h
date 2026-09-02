/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/workspace_fingerprint.h
 *
 * PURPOSE:
 *   Combine repository, build, configuration and toolchain evidence into a deterministic workspace fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_WORKSPACE_FINGERPRINT_H
#define UMICOM_DEVELOPER_HELIX_WORKSPACE_FINGERPRINT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix workspace fingerprint data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixWorkspaceFingerprint {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixWorkspaceFingerprint;
/* Initialise a deterministic, empty workspace fingerprint record. */
void umi_helix_workspace_fingerprint_init(UmiHelixWorkspaceFingerprint *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_workspace_fingerprint_validate(const UmiHelixWorkspaceFingerprint *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_workspace_fingerprint_score(const UmiHelixWorkspaceFingerprint *record);
#ifdef __cplusplus
}
#endif
#endif
