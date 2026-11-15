/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_job_native.c
 *
 * PURPOSE:
 *   Compare background scans with synchronous analysis and preserve prior results on failed jobs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/job.h"
#include "umicom/codeguard/scanner.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

/* This isolated test process changes only its own current directory. Relative
 * ASCII ".." avoids adding a second UTF-8 path converter to the test fixture. */
static int ChangeToParent(void)
{
#ifdef _WIN32
    return _chdir("..");
#else
    return chdir("..");
#endif
}
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); exit(1); } } while (0)

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    static const char *cases[] = {"ordinary", "empty", "budget", "missing",
        "cancel-before", "copy-profile", "relative-root", "retained-result"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected], name)) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    char root[1024];
    const char *leaf = !strcmp(name, "empty") ? "empty" :
        !strcmp(name, "missing") ? "absent" : "workspace";
    CHECK(snprintf(root, sizeof root, "%s/%s", argv[2], leaf) > 0);
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();
    profile.scan_architecture = 0;
    profile.scan_source_names = 0;
    profile.scan_duplicates = 0;
    profile.max_line_length = SIZE_MAX;
    if (!strcmp(name, "budget") || !strcmp(name, "retained-result")) profile.max_file_bytes = 1U;
    const char *request_root = root;
    if (!strcmp(name, "relative-root")) request_root = "workspace";
    UmiCodeGuardJob *job = NULL;
    CHECK(UmiCodeGuardJobCreate(request_root, &profile, &job) == UMI_STATUS_OK);
    if (!strcmp(name, "relative-root"))
        CHECK(ChangeToParent() == 0);
    if (!strcmp(name, "copy-profile")) profile.scan_c = 0;
    UmiTaskQueueConfig queue_config = {.worker_count = 1U, .capacity = 2U};
    UmiTaskQueue *queue = NULL;
    CHECK(umi_task_queue_create(&queue_config, &queue) == UMI_STATUS_OK);
    if (!strcmp(name, "cancel-before")) {
        CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
        CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_INVALID_STATE);
    } else {
        CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_OK);
        CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    }
    UmiCodeGuardJobSnapshot snapshot;
    CHECK(UmiCodeGuardJobRead(job, &snapshot) == UMI_STATUS_OK);
    UmiCodeGuardResult *retained = NULL;
    CHECK(umi_codeguard_result_create(4U, &retained) == UMI_STATUS_OK);
    UmiCodeGuardFinding old = {.severity = UMI_CODEGUARD_LOW, .line = 1U};
    strcpy(old.rule_id, "PRIOR-COMPLETE");
    CHECK(umi_codeguard_result_add(retained, &old) == UMI_STATUS_OK);
    const UmiCodeGuardFinding *borrow = umi_codeguard_result_at(retained, 0U);
    UmiCodeGuardResult *candidate = NULL;
    UmiStatus status = UmiCodeGuardJobTake(job, &candidate);
    if (!strcmp(name, "budget") || !strcmp(name, "retained-result"))
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && snapshot.status == status);
    else if (!strcmp(name, "missing")) CHECK(status != UMI_STATUS_OK && snapshot.status == status);
    else if (!strcmp(name, "cancel-before"))
        CHECK(status == UMI_STATUS_CANCELLED && snapshot.state == UMI_TASK_CANCELLED);
    else {
        CHECK(status == UMI_STATUS_OK && candidate != NULL && snapshot.result_available);
        UmiCodeGuardResult *synchronous = NULL;
        CHECK(umi_codeguard_result_create(4U, &synchronous) == UMI_STATUS_OK);
        UmiCodeGuardConfig config = umi_codeguard_config_default(root);
        config.profile = profile;
        config.profile.scan_c = 1;
        CHECK(umi_codeguard_scan(&config, synchronous) == UMI_STATUS_OK);
        CHECK(umi_codeguard_result_count(candidate) == umi_codeguard_result_count(synchronous));
        CHECK(snapshot.summary.total == umi_codeguard_result_count(candidate));
        for (size_t i = 0U; i < umi_codeguard_result_count(candidate); ++i) {
            const UmiCodeGuardFinding *a = umi_codeguard_result_at(candidate, i);
            const UmiCodeGuardFinding *b = umi_codeguard_result_at(synchronous, i);
            CHECK(!strcmp(a->rule_id, b->rule_id) && a->line == b->line && a->column == b->column);
            CHECK(a->severity == b->severity && umi_path_equal(a->path, b->path));
        }
        if (!strcmp(name, "empty")) CHECK(snapshot.summary.total == 0U);
        else CHECK(snapshot.summary.total != 0U);
        CHECK(umi_codeguard_result_swap(retained, candidate) == UMI_STATUS_OK);
        umi_codeguard_result_destroy(synchronous);
    }
    if (status != UMI_STATUS_OK)
        CHECK(candidate == NULL && umi_codeguard_result_at(retained, 0U) == borrow &&
            !strcmp(borrow->rule_id, "PRIOR-COMPLETE"));
    CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK && job == NULL);
    umi_codeguard_result_destroy(candidate);
    umi_codeguard_result_destroy(retained);
    CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    umi_task_queue_destroy(queue);
    return 0;
}
