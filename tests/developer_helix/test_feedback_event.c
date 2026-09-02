/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_feedback_event.c
 *
 * PURPOSE:
 *   Regression coverage for represent one build/test/compiler/observability feedback event consumed by a helix loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/feedback_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixFeedbackEvent r; umi_helix_feedback_event_init(&r,"feedback_event"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_feedback_event_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_feedback_event_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
