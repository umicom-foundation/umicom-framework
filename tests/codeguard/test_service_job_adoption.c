/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_service_job_adoption.c
 *
 * PURPOSE:
 *   Verify explicit background result admission preserves retained reports on unsuccessful attempts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/service.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#define CHANGE_PARENT() _chdir("..")
#else
#include <unistd.h>
#define CHANGE_PARENT() chdir("..")
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); exit(1); } } while (0)

typedef struct WrongThread { UmiCodeGuardService *service; UmiCodeGuardJob *job; UmiStatus status; } WrongThread;
static int AdoptElsewhere(void *data)
{
    WrongThread *input = data;
    input->status = umi_codeguard_service_adopt_job(input->service, input->job);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    const char *cases[] = {"arguments","success","empty","busy","cancelled","budget",
        "wrong-workspace","equivalent-root","repeat","already-taken","service-destroyed",
        "relative-drift","wrong-thread","default-profile"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(name, cases[selected])) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    char root[1024], alternate[1024];
    int written = snprintf(root, sizeof root, "%s/workspace", argv[2]);
    CHECK(written > 0 && (size_t)written < sizeof root);
    UmiCodeGuardService *service = NULL;
    CHECK(umi_codeguard_service_create(!strcmp(name,"relative-drift") ? "workspace" : root,
        &service) == UMI_STATUS_OK);
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();
    profile.scan_architecture = 0;
    profile.scan_source_names = 0;
    profile.scan_duplicates = 0;
    profile.max_line_length = SIZE_MAX;
    CHECK(umi_codeguard_service_scan(service, &profile) == UMI_STATUS_OK);
    const UmiCodeGuardResult *retained = umi_codeguard_service_result(service);
    const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(retained, 0U);
    size_t before = umi_codeguard_result_count(retained);
    CHECK(before != 0U && borrow != NULL);
    /* Copy object bytes explicitly so a padding byte is not part of an
     * unspecified structure-assignment comparison in this preservation check. */
#if 0
    UmiCodeGuardFinding saved = *borrow;
#endif
    UmiCodeGuardFinding saved;
    memcpy(&saved, borrow, sizeof saved);
    if (!strcmp(name,"empty")) profile.scan_c = 0;
    if (!strcmp(name,"budget")) profile.max_file_bytes = 1U;
    UmiCodeGuardJob *job = NULL;
    CHECK(umi_codeguard_service_create_job(service,
        !strcmp(name,"default-profile") ? NULL : &profile, &job) == UMI_STATUS_OK);
    if (!strcmp(name,"arguments")) {
        UmiCodeGuardJob *output = job;
        CHECK(umi_codeguard_service_create_job(NULL,&profile,&output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(output == NULL);
        CHECK(umi_codeguard_service_create_job(service,&profile,NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_adopt_job(NULL,job) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_service_adopt_job(service,NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    /* Release before scheduling: the worker must own all input independently. */
    if (!strcmp(name,"service-destroyed")) {
        umi_codeguard_service_destroy(service);
        service = NULL;
    }
    UmiTaskQueue *queue = NULL;
    UmiCodeGuardResult *taken = NULL;
    UmiCodeGuardService *other = NULL;
    UmiCodeGuardService *destination = service;
    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(name,"busy") || !strcmp(name,"arguments")) expected = UMI_STATUS_BUSY;
    else if (!strcmp(name,"cancelled")) {
        CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
        expected = UMI_STATUS_CANCELLED;
    } else {
        UmiTaskQueueConfig config = {.worker_count=1U,.capacity=2U};
        CHECK(umi_task_queue_create(&config,&queue) == UMI_STATUS_OK);
        CHECK(UmiCodeGuardJobSubmit(job,queue) == UMI_STATUS_OK);
        CHECK(umi_task_queue_wait_idle(queue,10000U) == UMI_STATUS_OK);
    }
    if (!strcmp(name,"budget")) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (!strcmp(name,"wrong-workspace") || !strcmp(name,"equivalent-root")) {
        written = snprintf(alternate,sizeof alternate,
            !strcmp(name,"wrong-workspace") ? "%s/other" : "%s/workspace/./sub/..",argv[2]);
        CHECK(written > 0 && (size_t)written < sizeof alternate);
        CHECK(umi_codeguard_service_create(alternate,&other) == UMI_STATUS_OK);
        destination = other;
        expected = !strcmp(name,"wrong-workspace") ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    }
    if (!strcmp(name,"relative-drift")) {
        CHECK(CHANGE_PARENT() == 0);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (!strcmp(name,"already-taken")) {
        CHECK(UmiCodeGuardJobTake(job,&taken) == UMI_STATUS_OK && taken != NULL);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (!strcmp(name,"service-destroyed")) {
        /* No service pointer is used after release; the independent job survives. */
        /* Earlier coverage destroyed only after completion; retain that sequence
         * for review while exercising the stronger pre-dispatch release above. */
#if 0
        umi_codeguard_service_destroy(service);
        service = NULL;
#endif
        CHECK(UmiCodeGuardJobTake(job,&taken) == UMI_STATUS_OK && taken != NULL);
        CHECK(umi_codeguard_result_count(taken) == before);
    } else {
        if (!strcmp(name,"wrong-thread")) {
            WrongThread input = {.service=service,.job=job,.status=UMI_STATUS_OK};
            UmiThread *thread = NULL;
            CHECK(umi_thread_start(AdoptElsewhere,&input,&thread) == UMI_STATUS_OK);
            int code = -1;
            CHECK(umi_thread_join(thread,&code) == UMI_STATUS_OK && code == 0);
            umi_thread_destroy(thread);
            CHECK(input.status == UMI_STATUS_INVALID_STATE);
            CHECK(umi_codeguard_result_at(retained,0U) == borrow);
            CHECK(!memcmp(borrow,&saved,sizeof saved));
        }
        UmiStatus status = umi_codeguard_service_adopt_job(destination,job);
        CHECK(status == expected);
        if (expected != UMI_STATUS_OK || destination != service) {
            CHECK(umi_codeguard_service_result(service) == retained);
            CHECK(umi_codeguard_result_count(retained) == before);
            CHECK(umi_codeguard_result_at(retained,0U) == borrow);
            CHECK(!memcmp(borrow,&saved,sizeof saved));
        } else {
            CHECK(umi_codeguard_service_result(service) == retained);
            UmiCodeGuardJobSnapshot completed;
            CHECK(UmiCodeGuardJobRead(job,&completed) == UMI_STATUS_OK);
            CHECK(!completed.result_available);
            CHECK(umi_codeguard_result_count(retained) == completed.summary.total);
            if (!strcmp(name,"empty")) CHECK(completed.summary.total == 0U);
            if (strcmp(name,"empty")) CHECK(umi_codeguard_result_count(retained) != 0U);
        }
        if (!strcmp(name,"repeat")) {
            const UmiCodeGuardFinding *published = umi_codeguard_result_at(retained,0U);
            CHECK(umi_codeguard_service_adopt_job(service,job) == UMI_STATUS_NOT_FOUND);
            CHECK(umi_codeguard_result_at(retained,0U) == published);
        }
        if (!strcmp(name,"wrong-workspace")) {
            CHECK(umi_codeguard_service_summary(other).total == 0U);
            UmiCodeGuardJobSnapshot snapshot;
            CHECK(UmiCodeGuardJobRead(job,&snapshot) == UMI_STATUS_OK && snapshot.result_available);
            CHECK(umi_codeguard_service_adopt_job(service,job) == UMI_STATUS_OK);
        }
        if (!strcmp(name,"relative-drift")) {
            CHECK(UmiCodeGuardJobTake(job,&taken) == UMI_STATUS_OK);
            CHECK(umi_codeguard_result_count(taken) == before);
        }
    }
    CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK);
    umi_codeguard_result_destroy(taken);
    umi_codeguard_service_destroy(other);
    umi_codeguard_service_destroy(service);
    if (queue != NULL) {
        CHECK(umi_task_queue_shutdown(queue,0) == UMI_STATUS_OK);
        umi_task_queue_destroy(queue);
    }
    return 0;
}
