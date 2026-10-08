/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_capture.c
 * PURPOSE: Capture real queued disabled-test selections without launching child processes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiCtestJobRequest requests[2] = {0};
    for (size_t i = 0; i < 2U; ++i)
    {
        (void)snprintf(requests[i].test_id, sizeof(requests[i].test_id), "disabled.%zu", i);
        (void)snprintf(requests[i].name, sizeof(requests[i].name), "Disabled %zu", i);
        strcpy(requests[i].build_directory, "/fixture/unused");
        requests[i].enabled = 0;
    }
    UmiCtestJobOptions options = {2, 1};
    UmiCtestJob *job = NULL;
    CHECK(UmiCtestJobCreate(requests, 2, &options, &job) == UMI_STATUS_OK);
    if (strcmp(argv[1], "selection") == 0)
    {
        requests[0].name[0] = 'x';
        UmiCtestJobPlanSnapshot plan = {0};
        UmiCtestJobRequest copied = {0};
        CHECK(UmiCtestJobReadPlan(job, &plan) == UMI_STATUS_OK && plan.request_count == 2 &&
              plan.repeat_count == 2 && plan.stop_on_failure);
        CHECK(UmiCtestJobRequestAt(job, 0, &copied) == UMI_STATUS_OK &&
              strcmp(copied.name, "Disabled 0") == 0);
        CHECK(UmiCtestJobRequestAt(job, 2, &copied) == UMI_STATUS_NOT_FOUND &&
              strcmp(copied.name, "Disabled 0") == 0);
    }
    else
    {
        bool cancel = strcmp(argv[1], "cancelled") == 0;
        CHECK(cancel || strcmp(argv[1], "disabled") == 0);
        UmiTaskQueue *queue = NULL;
        UmiTaskQueueConfig config = umi_task_queue_config_default();
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
        if (cancel)
            CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
        else
        {
            CHECK(UmiCtestJobSubmit(job, queue) == UMI_STATUS_OK);
            CHECK(umi_task_queue_wait_idle(queue, 5000) == UMI_STATUS_OK);
        }
        UmiDataServer *server = NULL;
        UmiTestArchive *archive = NULL;
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveCreate(server, "actual", &archive) == UMI_STATUS_OK);
        UmiTestArchiveOrigin origin = {0};
        strcpy(origin.source_root, "/fixture/source");
        UmiTestArchiveEntry entry = {0};
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK);
        CHECK(entry.run.planned == 4 && entry.run.completed == (cancel ? 0U : 4U) && entry.run.passed == 0 &&
              entry.run.skipped == (cancel ? 0U : 4U));
        UmiTestArchiveDestroy(archive);
        umi_data_server_destroy(server);
        umi_task_queue_destroy(queue);
    }
    CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK);
    return 0;
}
