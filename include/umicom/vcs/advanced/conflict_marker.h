/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/conflict_marker.h
 *
 * PURPOSE:
 *   Scan textual merge-marker lines without requiring editor-specific parsing.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_CONFLICT_MARKER_H
#define UMICOM_VCS_ADVANCED_CONFLICT_MARKER_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiVcsAdvancedConflictMarkerKind {
    UMI_VCS_MARKER_NONE=0, UMI_VCS_MARKER_OURS=1, UMI_VCS_MARKER_BASE=2,
    UMI_VCS_MARKER_SEPARATOR=3, UMI_VCS_MARKER_THEIRS=4
} UmiVcsAdvancedConflictMarkerKind;
UmiVcsAdvancedConflictMarkerKind umi_vcs_advanced_conflict_marker_classify(const char *line);
int umi_vcs_advanced_conflict_marker_balanced(const char *text,size_t *out_conflicts);
#ifdef __cplusplus
}
#endif
#endif
