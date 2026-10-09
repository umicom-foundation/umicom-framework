/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/working_tree_job.h
 * PURPOSE: Own a cancellable background Git observation independently of frontend controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKING_TREE_JOB_H
#define UMICOM_VCS_WORKING_TREE_JOB_H
#include "umicom/platform/task_queue.h"
#include "umicom/vcs/working_tree.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /** Own the copied repository request and its one-shot background task. */
    typedef struct UmiVcsWorkingTreeJob UmiVcsWorkingTreeJob;

    /** Report in-memory progress without waiting for Git to finish. */
    typedef struct UmiVcsWorkingTreeJobSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
    } UmiVcsWorkingTreeJobSnapshot;

    /**
     * Copy the request without launching Git. The task owns cancellation, so pass no external
     * token. Create, Submit, Take and Destroy belong to the creating thread. A null program selects
     * Git from the executable search path. On failure the output pointer is unchanged.
     */
    UmiStatus UmiVcsWorkingTreeJobCreate(const UmiVcsWorkingTreeRequest *request,
                                         UmiVcsWorkingTreeJob **out_job);

    /** Submit once to a shared queue. A rejected submission leaves the task unexecuted. */
    UmiStatus UmiVcsWorkingTreeJobSubmit(UmiVcsWorkingTreeJob *job, UmiTaskQueue *queue);

    /** Request cancellation without blocking the caller or claiming the child process has exited.
     */
    UmiStatus UmiVcsWorkingTreeJobCancel(UmiVcsWorkingTreeJob *job);

    /** Copy state while the caller keeps the job alive. This never waits for process output. */
    UmiStatus UmiVcsWorkingTreeJobRead(UmiVcsWorkingTreeJob *job,
                                       UmiVcsWorkingTreeJobSnapshot *out);

    /**
     * Transfer the completed observation once. BUSY or failure preserves out_tree.
     * A cancelled task never publishes its result, even if cancellation arrived after Git exited.
     */
    UmiStatus UmiVcsWorkingTreeJobTake(UmiVcsWorkingTreeJob *job, UmiVcsWorkingTree **out_tree);

    /** Release a created or terminal job. BUSY preserves queued/running work; cancel and poll
     * first. */
    UmiStatus UmiVcsWorkingTreeJobDestroy(UmiVcsWorkingTreeJob *job);

#ifdef __cplusplus
}
#endif
#endif
