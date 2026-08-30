/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_preview_session.c
 *
 * PURPOSE:
 *   Validate track preview lifecycle and source/document revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/preview_session.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPreviewTarget t;UmiRadPreviewSession s;CHECK(umi_rad_preview_target_init(&t)==UMI_STATUS_OK);CHECK(umi_rad_preview_session_init(&s,"preview",&t)==UMI_STATUS_OK);CHECK(umi_rad_preview_session_start(&s,3U)==UMI_STATUS_OK);CHECK(umi_rad_preview_session_rendered(&s,3U)==UMI_STATUS_OK);return 0;}
