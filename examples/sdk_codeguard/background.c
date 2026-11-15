/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_codeguard/background.c
 *
 * PURPOSE:
 *   Demonstrate owned background CodeGuard scanning through an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/codeguard.h"
#include <stdio.h>
#include <string.h>

/* A console host can wait for its private queue. An interactive application
 * instead keeps its shared queue alive, polls JobRead from its event loop and
 * transfers a completed result on the owner thread. Neither host duplicates
 * scan rules, native file reading or cancellation inside application code. */
/* Add an optional retained-service workflow while preserving direct one-shot transfer and cancellation. The previous implementation remains for review. */
#if 0
int main(int argc, char **argv)
{
    if ((argc != 2 && argc != 3) ||
        (argc == 3 && strcmp(argv[2], "--cancel-before-submit") != 0)) {
        fputs("Usage: umicom-sdk-codeguard-background <root> [--cancel-before-submit]\n", stderr);
        return 2;
    }
    UmiCodeGuardProfile profile = umi_codeguard_profile_security();
    UmiCodeGuardJob *job = NULL;
    UmiStatus status = UmiCodeGuardJobCreate(argv[1], &profile, &job);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create scan job (status %d).\n", (int)status);
        return 2;
    }
    UmiTaskQueueConfig configuration = {.worker_count = 1U, .capacity = 1U};
    UmiTaskQueue *queue = NULL;
    status = umi_task_queue_create(&configuration, &queue);
    if (status != UMI_STATUS_OK) {
        (void)UmiCodeGuardJobDestroy(&job);
        fputs("Cannot create the example's worker queue.\n", stderr);
        return 2;
    }
    if (argc == 3) status = UmiCodeGuardJobCancel(job);
    else status = UmiCodeGuardJobSubmit(job, queue);
    /* Shutdown drains accepted work and joins this console example's worker.
     * Do not perform a blocking shutdown on an application's GUI thread. */
    UmiStatus stopped = umi_task_queue_shutdown(queue, 0);
    if (stopped != UMI_STATUS_OK) {
        fprintf(stderr, "Worker shutdown failed (status %d); ownership could not be released safely.\n",
            (int)stopped);
        return 3;
    }
    UmiCodeGuardResult *result = NULL;
    if (status == UMI_STATUS_OK) status = UmiCodeGuardJobTake(job, &result);
    int exit_code = 3;
    if (status == UMI_STATUS_CANCELLED) {
        fputs("Scan cancelled; no new result transferred.\n", stderr);
        exit_code = 4;
    } else if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Scan did not complete (status %d); no report published.\n", (int)status);
    } else {
        UmiCodeGuardSummary summary = umi_codeguard_summary_build(result);
        status = umi_codeguard_report_write(stdout, UMI_CODEGUARD_REPORT_TEXT, result);
        if (status == UMI_STATUS_OK && fflush(stdout) == 0)
            exit_code = umi_codeguard_summary_failed(&summary, profile.fail_on) ? 1 : 0;
        else fputs("Could not finish writing the completed report.\n", stderr);
    }
    umi_codeguard_result_destroy(result);
    status = UmiCodeGuardJobDestroy(&job);
    umi_task_queue_destroy(queue);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot release terminal job (status %d).\n", (int)status);
        return 3;
    }
    return exit_code;
}
#endif
int main(int argc, char **argv)
{
    int adopt = 0, cancel = 0;
    if (argc < 2 || argc > 4) {
        fputs("Usage: umicom-sdk-codeguard-background <root> [--adopt] [--cancel-before-submit]\n", stderr);
        return 2;
    }
    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "--adopt") && !adopt) adopt = 1;
        else if (!strcmp(argv[i], "--cancel-before-submit") && !cancel) cancel = 1;
        else { fprintf(stderr, "Unknown or repeated option: %s\n", argv[i]); return 2; }
    }
    UmiCodeGuardProfile profile = umi_codeguard_profile_security();
    UmiCodeGuardJob *job = NULL;
    UmiCodeGuardService *service = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if (adopt) {
        status = umi_codeguard_service_create(argv[1], &service);
        if (status == UMI_STATUS_OK)
            status = umi_codeguard_service_create_job(service, &profile, &job);
    } else status = UmiCodeGuardJobCreate(argv[1], &profile, &job);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create scan job (status %d).\n", (int)status);
        umi_codeguard_service_destroy(service);
        return 2;
    }
    UmiTaskQueueConfig configuration = {.worker_count = 1U, .capacity = 1U};
    UmiTaskQueue *queue = NULL;
    status = umi_task_queue_create(&configuration, &queue);
    if (status != UMI_STATUS_OK) {
        (void)UmiCodeGuardJobDestroy(&job);
        umi_codeguard_service_destroy(service);
        fputs("Cannot create the example's worker queue.\n", stderr);
        return 2;
    }
    status = cancel ? UmiCodeGuardJobCancel(job) : UmiCodeGuardJobSubmit(job, queue);
    /* This console owns a private queue. A GUI instead polls and keeps its shared
     * queue alive; blocking shutdown is not an event-loop integration pattern. */
    UmiStatus stopped = umi_task_queue_shutdown(queue, 0);
    if (stopped != UMI_STATUS_OK) {
        fprintf(stderr, "Worker shutdown failed (status %d); ownership could not be released safely.\n",
            (int)stopped);
        return 3;
    }
    UmiCodeGuardResult *owned = NULL;
    const UmiCodeGuardResult *result = NULL;
    if (status == UMI_STATUS_OK) {
        if (adopt) {
            status = umi_codeguard_service_adopt_job(service, job);
            if (status == UMI_STATUS_OK) result = umi_codeguard_service_result(service);
        } else {
            status = UmiCodeGuardJobTake(job, &owned);
            result = owned;
        }
    }
    int exit_code = 3;
    if (status == UMI_STATUS_CANCELLED) {
        fputs("Scan cancelled; no new result transferred.\n", stderr);
        exit_code = 4;
    } else if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Scan did not complete (status %d); no report published.\n", (int)status);
    } else {
        UmiCodeGuardSummary summary = umi_codeguard_summary_build(result);
        status = umi_codeguard_report_write(stdout, UMI_CODEGUARD_REPORT_TEXT, result);
        if (status == UMI_STATUS_OK && fflush(stdout) == 0)
            exit_code = umi_codeguard_summary_failed(&summary, profile.fail_on) ? 1 : 0;
        else fputs("Could not finish writing the completed report.\n", stderr);
    }
    /* The direct result is owned; the service result is only borrowed. Destroy
     * exactly the matching owner after the report writer stops using it. */
    umi_codeguard_result_destroy(owned);
    umi_codeguard_service_destroy(service);
    status = UmiCodeGuardJobDestroy(&job);
    umi_task_queue_destroy(queue);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot release terminal job (status %d).\n", (int)status);
        return 3;
    }
    return exit_code;
}
