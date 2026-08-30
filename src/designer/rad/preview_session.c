/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/preview_session.c
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
#include "umicom/designer/rad/preview_session.h"
#include <string.h>
UmiStatus umi_rad_preview_session_init(UmiRadPreviewSession *s,const char *id,const UmiRadPreviewTarget *t){if(s==NULL||!umi_rad_id_valid(id)||t==NULL||!umi_rad_preview_target_is_valid(t))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_rad_copy_text(s->session_id,sizeof s->session_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->target=*t;return UMI_STATUS_OK;}
UmiStatus umi_rad_preview_session_start(UmiRadPreviewSession *s,uint64_t r){if(s==NULL||s->lifecycle==UMI_RAD_PREVIEW_RUNNING)return UMI_STATUS_INVALID_STATE;s->source_revision=r;s->lifecycle=UMI_RAD_PREVIEW_RUNNING;return UMI_STATUS_OK;}
UmiStatus umi_rad_preview_session_rendered(UmiRadPreviewSession *s,uint64_t r){if(s==NULL||s->lifecycle!=UMI_RAD_PREVIEW_RUNNING||r>s->source_revision)return UMI_STATUS_INVALID_STATE;s->rendered_revision=r;return UMI_STATUS_OK;}
