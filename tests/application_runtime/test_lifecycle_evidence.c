/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_lifecycle_evidence.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Exercise test lifecycle evidence and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_lifecycle_evidence(void){ UmiApplicationLifecycleEvidence e; umi_application_lifecycle_evidence_init(&e); assert(umi_application_lifecycle_evidence_record(&e,UMI_APPLICATION_LIFECYCLE_STARTED,UMI_STATUS_OK)==UMI_STATUS_OK); assert(umi_application_lifecycle_evidence_last(&e)!=NULL); return 0; }
