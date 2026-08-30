/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/types.h
 *
 * PURPOSE:
 *   Define stable, bounded presentation and lifecycle types for the operational
 *   diff, merge, review and partial-staging workbench.
 *
 * ARCHITECTURE:
 *   This layer composes the established developer-productivity and advanced
 *   VCS engines. It does not introduce another diff algorithm or Git provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_TYPES_H
#define UMICOM_VCS_WORKBENCH_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_VCS_WORKBENCH_API_VERSION 1U
#define UMI_VCS_WORKBENCH_ID_CAPACITY 96U
#define UMI_VCS_WORKBENCH_LABEL_CAPACITY 160U
#define UMI_VCS_WORKBENCH_TEXT_CAPACITY 512U
#define UMI_VCS_WORKBENCH_MAX_DIRECTORY_ENTRIES 256U
#define UMI_VCS_WORKBENCH_MAX_DIFFERENCE_BANDS 256U
#define UMI_VCS_WORKBENCH_MAX_REVIEW_THREADS 128U
#define UMI_VCS_WORKBENCH_MAX_REVIEW_COMMENTS 256U

typedef enum UmiVcsWorkbenchPhase {
    UMI_VCS_WORKBENCH_IDLE = 0,
    UMI_VCS_WORKBENCH_COMPARING = 1,
    UMI_VCS_WORKBENCH_DIRECTORY_COMPARING = 2,
    UMI_VCS_WORKBENCH_MERGING = 3,
    UMI_VCS_WORKBENCH_PARTIAL_STAGING = 4,
    UMI_VCS_WORKBENCH_REVIEWING = 5,
    UMI_VCS_WORKBENCH_FAILED = 6
} UmiVcsWorkbenchPhase;

typedef enum UmiVcsWorkbenchCompareViewMode {
    UMI_VCS_WORKBENCH_SIDE_BY_SIDE = 0,
    UMI_VCS_WORKBENCH_INLINE = 1
} UmiVcsWorkbenchCompareViewMode;

typedef enum UmiVcsWorkbenchReviewDecision {
    UMI_VCS_WORKBENCH_REVIEW_PENDING = 0,
    UMI_VCS_WORKBENCH_REVIEW_APPROVED = 1,
    UMI_VCS_WORKBENCH_REVIEW_CHANGES_REQUESTED = 2
} UmiVcsWorkbenchReviewDecision;

const char *umi_vcs_workbench_phase_text(UmiVcsWorkbenchPhase phase);
const char *umi_vcs_workbench_review_decision_text(
    UmiVcsWorkbenchReviewDecision decision);
UmiStatus umi_vcs_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_TYPES_H */
