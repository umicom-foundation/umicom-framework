/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_capture.h
 * PURPOSE: Copy the exact selection behind a CTest job without executing it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_CAPTURE_H
#define UMICOM_TESTING_CTEST_CAPTURE_H
#include <stdbool.h>
#include "umicom/testing/ctest_job.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiCtestJobPlanSnapshot
    {
        size_t request_count;
        uint32_t repeat_count;
        bool stop_on_failure;
    } UmiCtestJobPlanSnapshot;
    /* Selection storage is immutable after creation. These calls copy it without
 * changing the run, reading files or granting execution authority. Keep the job
 * alive throughout each call. RequestAt uses selection order, not attempt order.
 * Failure preserves the caller's output; unknown indices return NOT_FOUND. */
    UmiStatus UmiCtestJobReadPlan(UmiCtestJob *job, UmiCtestJobPlanSnapshot *out_plan);
    UmiStatus UmiCtestJobRequestAt(UmiCtestJob *job, size_t index, UmiCtestJobRequest *out_request);
#ifdef __cplusplus
}
#endif
#endif
