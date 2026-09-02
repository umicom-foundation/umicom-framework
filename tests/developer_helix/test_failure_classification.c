/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_failure_classification.c
 *
 * PURPOSE:
 *   Regression coverage for classify observed failures into build, test, semantic, infrastructure or policy categories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/failure_classification.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixFailureClassification r; umi_helix_failure_classification_init(&r,"failure_classification"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_failure_classification_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_failure_classification_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
