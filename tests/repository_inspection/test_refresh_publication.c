/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_refresh_publication.c
 * PURPOSE: Keep repository reports coherent when collection or evaluation fails.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/inspection_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static void *TestAllocate(size_t bytes);
static UmiStatus Collect(const UmiRepositoryInspectionContext *context, UmiRepositoryInspectionSnapshot *snapshot);
static UmiStatus Evaluate(UmiRepositoryMaintenanceService *service);
static UmiStatus Report(const UmiRepositoryInspectionSnapshot *snapshot,
    const UmiRepositoryInspectionPolicy *policy, UmiRepositoryInspectionReport *report);
#define malloc TestAllocate
#define umi_repository_snapshot_collect Collect
#define umi_repository_maintenance_service_evaluate Evaluate
#define umi_repository_inspection_report_build Report
#include "../../src/repository/inspection/inspection_service.c"
#undef malloc
#undef umi_repository_snapshot_collect
#undef umi_repository_maintenance_service_evaluate
#undef umi_repository_inspection_report_build

static void *TestAllocate(size_t bytes)
{
    return strcmp(scenario, "allocation") == 0 ? NULL : malloc(bytes);
}
static UmiStatus Collect(const UmiRepositoryInspectionContext *context, UmiRepositoryInspectionSnapshot *snapshot)
{
    (void)context; ++calls;
    snapshot->maintenance.worktree.modified = 7U;
    return strcmp(scenario, "collect") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Evaluate(UmiRepositoryMaintenanceService *service)
{
    ++calls; ++service->revision;
    service->report.errors = 3U;
    return strcmp(scenario, "evaluate") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
static UmiStatus Report(const UmiRepositoryInspectionSnapshot *snapshot,
    const UmiRepositoryInspectionPolicy *policy, UmiRepositoryInspectionReport *report)
{
    (void)snapshot; (void)policy; ++calls;
    report->errors = 4U;
    return strcmp(scenario, "report") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

/* A failed dependency deliberately writes to its candidate before returning an
 * error. The live service must still retain its previous coherent evidence. */
int main(int argc, char **argv)
{
    int failed = 0;
    UmiRepositoryInspectionService *service = calloc(1U, sizeof *service);
    UmiRepositoryInspectionService *before = calloc(1U, sizeof *before);
    CHECK(argc == 2 && service != NULL && before != NULL);
    scenario = argv[1];
    umi_repository_inspection_service_init(service);
    service->revision = 9U;
    service->snapshot.revision = 9U;
    service->snapshot.maintenance.worktree.modified = 2U;
    service->maintenance.summary.worktree.modified = 2U;
    service->report.errors = 1U;
    if (strcmp(scenario, "revision") == 0) service->revision = UINT64_MAX;
    if (strcmp(scenario, "maintenance_revision") == 0) service->maintenance.revision = UINT64_MAX;
    *before = *service;
    UmiRepositoryInspectionContext context = {NULL, "unused", 100U};
    if (strcmp(scenario, "null") == 0) {
        CHECK(umi_repository_inspection_service_refresh(NULL, &context) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_repository_inspection_service_refresh(service, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        before->last_status = UMI_STATUS_INVALID_ARGUMENT;
        CHECK(memcmp(before, service, sizeof *service) == 0 && calls == 0U);
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "allocation") == 0) expected = UMI_STATUS_OUT_OF_MEMORY;
    if (strcmp(scenario, "collect") == 0) expected = UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "evaluate") == 0) expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(scenario, "report") == 0 || strcmp(scenario, "revision") == 0 ||
        strcmp(scenario, "maintenance_revision") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    CHECK(umi_repository_inspection_service_refresh(service, &context) == expected);
    if (expected != UMI_STATUS_OK) {
        before->last_status = expected;
        CHECK(memcmp(before, service, sizeof *service) == 0);
    } else {
        CHECK(service->revision == 10U && service->snapshot.revision == 10U);
        CHECK(service->snapshot.maintenance.worktree.modified == 7U);
        CHECK(service->maintenance.summary.worktree.modified == 7U);
        CHECK(service->maintenance.report.errors == 3U && service->report.errors == 4U);
        CHECK(service->maintenance.revision == before->maintenance.revision + 1U);
    }
    if (strcmp(scenario, "allocation") == 0 || strcmp(scenario, "revision") == 0 ||
        strcmp(scenario, "maintenance_revision") == 0) CHECK(calls == 0U);
    else if (strcmp(scenario, "collect") == 0) CHECK(calls == 1U);
    else if (strcmp(scenario, "evaluate") == 0) CHECK(calls == 2U);
    else CHECK(calls == 3U);
cleanup:
    free(before); free(service);
    return failed;
}
