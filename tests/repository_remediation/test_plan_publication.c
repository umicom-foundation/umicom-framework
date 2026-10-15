/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_remediation/test_plan_publication.c
 * PURPOSE: Publish repository proposals together with their accepted inspection evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/remediation_service.h"
#include "umicom/repository/remediation_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static void *Allocate(size_t size);
static UmiStatus Refresh(UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context);
static UmiStatus Build(const UmiRepositoryDoctorReport *doctor,
    const UmiRepositoryInspectionReport *inspection, UmiRepositoryRemediationPlan *plan);
static UmiStatus Report(const UmiRepositoryRemediationPlan *plan,
    const UmiRepositoryRemediationPolicy *policy, UmiRepositoryRemediationReport *report);
#define malloc Allocate
#define umi_repository_inspection_service_refresh Refresh
#define umi_repository_remediation_builder_build Build
#define umi_repository_remediation_report_build Report
#include "../../src/repository/remediation/remediation_service.c"
#undef malloc
#undef umi_repository_inspection_service_refresh
#undef umi_repository_remediation_builder_build
#undef umi_repository_remediation_report_build

/* Dependency failures deliberately change candidate data before failing. The
 * previous service must remain intact, apart from its latest attempt status. */
static void *Allocate(size_t size)
{
    return strcmp(scenario, "allocation") == 0 ? NULL : malloc(size);
}
static UmiStatus Refresh(UmiRepositoryInspectionService *service,
    const UmiRepositoryInspectionContext *context)
{
    (void)context; ++calls;
    ++service->revision; service->snapshot.revision = service->revision;
    service->snapshot.tracked_gitlinks = 9U;
    return strcmp(scenario, "refresh") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Build(const UmiRepositoryDoctorReport *doctor,
    const UmiRepositoryInspectionReport *inspection, UmiRepositoryRemediationPlan *plan)
{
    (void)doctor; (void)inspection; ++calls;
    plan->safe_count = 4U;
    return strcmp(scenario, "build") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus Report(const UmiRepositoryRemediationPlan *plan,
    const UmiRepositoryRemediationPolicy *policy, UmiRepositoryRemediationReport *report)
{
    (void)plan; (void)policy; ++calls;
    report->manual_review = 4U;
    return strcmp(scenario, "report") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

int main(int argc, char **argv)
{
    int failed = 0;
    UmiRepositoryRemediationService *service = calloc(1U, sizeof *service);
    UmiRepositoryRemediationService *before = calloc(1U, sizeof *before);
    CHECK(argc == 2 && service != NULL && before != NULL);
    scenario = argv[1];
    umi_repository_remediation_service_init(service);
    service->revision = 4U;
    service->inspection.revision = service->inspection.snapshot.revision = 2U;
    service->plan.safe_count = 1U;
    if (strcmp(scenario, "revision") == 0) service->revision = UINT64_MAX;
    *before = *service;
    UmiRepositoryInspectionContext context = {NULL, "unused", 100U};
    UmiStatus expected = strcmp(scenario, "allocation") == 0 ? UMI_STATUS_OUT_OF_MEMORY :
        strcmp(scenario, "refresh") == 0 ? UMI_STATUS_IO_ERROR :
        strcmp(scenario, "build") == 0 || strcmp(scenario, "revision") == 0
            ? UMI_STATUS_CAPACITY_EXCEEDED :
        strcmp(scenario, "report") == 0 ? UMI_STATUS_INVALID_STATE :
        strcmp(scenario, "null") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    if (strcmp(scenario, "null") == 0)
        CHECK(umi_repository_remediation_service_plan(NULL, &context) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_repository_remediation_service_plan(service,
        strcmp(scenario, "null") == 0 ? NULL : &context) == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(calls == 3U && service->revision == 5U);
        CHECK(service->inspection.revision == 3U && service->inspection.snapshot.tracked_gitlinks == 9U);
        CHECK(service->plan.safe_count == 4U && service->report.manual_review == 4U);
        CHECK(UmiRepositoryRemediationEvidenceCurrent(service));
    } else {
        before->last_status = expected;
        CHECK(memcmp(before, service, sizeof *service) == 0);
        CHECK(!UmiRepositoryRemediationEvidenceCurrent(service));
        unsigned expectedCalls = strcmp(scenario, "refresh") == 0 ? 1U :
            strcmp(scenario, "build") == 0 ? 2U : strcmp(scenario, "report") == 0 ? 3U : 0U;
        CHECK(calls == expectedCalls);
    }
cleanup:
    free(before); free(service);
    return failed;
}
