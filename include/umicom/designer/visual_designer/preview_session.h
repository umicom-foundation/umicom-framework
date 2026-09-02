/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/preview_session.h
 *
 * PURPOSE:
 *   Track preview lifecycle and source/document revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_SESSION_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_SESSION_H
#include "umicom/designer/visual_designer/preview_target.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named visual designer preview lifecycle values accepted by this public contract.
 */
typedef enum UmiRadPreviewLifecycle { UMI_RAD_PREVIEW_IDLE=0, UMI_RAD_PREVIEW_STARTING=1, UMI_RAD_PREVIEW_RUNNING=2, UMI_RAD_PREVIEW_STOPPED=3 } UmiRadPreviewLifecycle;
/**
 * Represent the visual designer preview session data shared with callers of this public contract.
 */
typedef struct UmiRadPreviewSession { char session_id[UMI_RAD_ID_CAPACITY]; UmiRadPreviewTarget target; UmiRadPreviewLifecycle lifecycle; uint64_t source_revision; uint64_t rendered_revision; } UmiRadPreviewSession;
/**
 * Initialise visual designer preview session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_session_init(UmiRadPreviewSession *session,const char *session_id,const UmiRadPreviewTarget *target);
/**
 * Provide the visual designer preview session start operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_preview_session_start(UmiRadPreviewSession *session,uint64_t source_revision);
/**
 * Provide the visual designer preview session rendered operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_preview_session_rendered(UmiRadPreviewSession *session,uint64_t revision);
#ifdef __cplusplus
}
#endif
#endif
