/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/preview_session.h
 *
 * PURPOSE:
 *   Track preview lifecycle and source/document revisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PREVIEW_SESSION_H
#define UMICOM_DESIGNER_RAD_PREVIEW_SESSION_H
#include "umicom/designer/rad/preview_target.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiRadPreviewLifecycle { UMI_RAD_PREVIEW_IDLE=0, UMI_RAD_PREVIEW_STARTING=1, UMI_RAD_PREVIEW_RUNNING=2, UMI_RAD_PREVIEW_STOPPED=3 } UmiRadPreviewLifecycle;
typedef struct UmiRadPreviewSession { char session_id[UMI_RAD_ID_CAPACITY]; UmiRadPreviewTarget target; UmiRadPreviewLifecycle lifecycle; uint64_t source_revision; uint64_t rendered_revision; } UmiRadPreviewSession;
UmiStatus umi_rad_preview_session_init(UmiRadPreviewSession *session,const char *session_id,const UmiRadPreviewTarget *target);
UmiStatus umi_rad_preview_session_start(UmiRadPreviewSession *session,uint64_t source_revision);
UmiStatus umi_rad_preview_session_rendered(UmiRadPreviewSession *session,uint64_t revision);
#ifdef __cplusplus
}
#endif
#endif
