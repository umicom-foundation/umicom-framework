/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_output.h
 * PURPOSE: Expose copied CTest output while an attempt is still running.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_OUTPUT_H
#define UMICOM_TESTING_CTEST_OUTPUT_H
#include "umicom/testing/ctest_adapter.h"
#include "umicom/testing/ctest_job.h"
#include "umicom/platform/process.h"
#include "umicom/platform/output_tail.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CTEST_OUTPUT_CAPACITY 65536U

    /* Verbose CTest output is forwarded on the executing thread before it returns.
 * The observer borrows each raw chunk only during its call. It must not update
 * widgets, block, re-enter this run or release its context. Both streams merge.
 * NULL keeps the existing quiet invocation. A fresh verified report, timeout and
 * process-tree ownership still determine the final result; text is not a verdict.
 * CTest fixtures may emit output too, so bytes describe this invocation, not
 * necessarily only the selected test body. Test programs can buffer their output. */
    UmiStatus UmiCtestRunObserved(const char *build_directory, const char *test_name,
                                  const UmiCtestRunOptions *options, UmiProcessOutputObserver observer,
                                  void *context, UmiTestResult *out_result);

    /* One copied tail for the most recently started attempt. New attempts clear the
 * bytes while revision keeps increasing within this job. A slow reader may miss
 * earlier attempts; completed results remain available through ResultAt.
 * attempt counts repetitions, invocation counts all started selected attempts.
 * task_id identifies the job. invocation == 0 means no attempt has started.
 * Retention is bounded and is not a full log or a persistent output archive.
 * Allocate large snapshots on the heap on threads with limited stack space. */
    typedef struct UmiCtestOutputSnapshot
    {
        uint64_t task_id;
        uint64_t revision;
        size_t invocation;
        uint32_t attempt;
        char test_id[UMI_TEST_ID_CAPACITY];
        char name[UMI_TEST_NAME_CAPACITY];
        UmiOutputTailState tail;
        UmiStatus capture_status;
        UmiStatus result_status;
        UmiTestState result_state;
        bool attempt_complete;
        char bytes[UMI_CTEST_OUTPUT_CAPACITY];
    } UmiCtestOutputSnapshot;

    /* Copies under the same mutex as worker writes. No process or file I/O occurs.
 * May overlap execution, never destruction. Counters saturate; when the tail's
 * counters_saturated flag is true, frontends refresh even at equal revision.
 * A completed attempt's status does not describe attempts that never started. */
    UmiStatus UmiCtestJobReadOutput(UmiCtestJob *job, UmiCtestOutputSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
