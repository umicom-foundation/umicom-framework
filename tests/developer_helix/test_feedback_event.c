/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_feedback_event.c
 *
 * PURPOSE:
 *   Regression coverage for represent one build/test/compiler/observability feedback event consumed by a helix loop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/feedback_event.h"
int main(void){ UmiHelixFeedbackEvent r; umi_helix_feedback_event_init(&r,"feedback_event"); if(umi_helix_feedback_event_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_feedback_event_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
