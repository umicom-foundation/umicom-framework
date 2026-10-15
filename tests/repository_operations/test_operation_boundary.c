/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_operation_boundary.c
 * PURPOSE: Reject invalid operation requests and preserve failures without publishing partial reports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/repository_operation_service.h"
#include "umicom/repository/remediation_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static int context_valid;
static void *Allocate(size_t size);
static UmiStatus Plan(UmiRepositoryRemediationService *service,
    const UmiRepositoryInspectionContext *context);
static UmiStatus Format(UmiRepositoryOperationKind kind,
    const UmiRepositoryRemediationService *service, UmiRepositoryOperationResult *result);
#define malloc Allocate
#define umi_repository_remediation_service_plan Plan
#define umi_repository_operation_formatter_format Format
#include "../../src/repository/operations/repository_operation_service.c"
#undef malloc
#undef umi_repository_remediation_service_plan
#undef umi_repository_operation_formatter_format
static void *Allocate(size_t size)
{
    return strcmp(scenario, "allocation") == 0 ? NULL : malloc(size);
}
/* The seam supplies controlled observations; these cases must never launch Git. */
static UmiStatus Plan(UmiRepositoryRemediationService *service,
    const UmiRepositoryInspectionContext *context)
{
    ++calls;
    context_valid = service != NULL && context != NULL &&
        strcmp(context->repository_root, "chosen-repository") == 0;
    return strcmp(scenario, "plan") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Format(UmiRepositoryOperationKind kind,
    const UmiRepositoryRemediationService *service, UmiRepositoryOperationResult *result)
{
    (void)kind; (void)service; ++calls;
    if (strcmp(scenario, "format") == 0) return UMI_STATUS_CAPACITY_EXCEEDED;
    result->issue_count = 3U;
    return UMI_STATUS_OK;
}
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); free(result); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    UmiRepositoryOperationResult *result = malloc(sizeof *result);
    CHECK(argc == 2 && result != NULL);
    scenario = argv[1];
    memset(result, 0x5a, sizeof *result);
    UmiRepositoryOperationRequest request = {
        .kind = UMI_REPOSITORY_OPERATION_STATUS, .repository_root = "chosen-repository"
    };
    if (strcmp(scenario, "invalid_kind") == 0) request.kind = (UmiRepositoryOperationKind)99;
    if (strcmp(scenario, "empty_root") == 0) request.repository_root = "";
    if (strcmp(scenario, "null_root") == 0) request.repository_root = NULL;
    if (strcmp(scenario, "null_output") == 0) {
        CHECK(umi_repository_operation_service_execute(&request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U);
        free(result); return 0;
    }
    UmiStatus expected = strcmp(scenario, "success") == 0 ? UMI_STATUS_OK :
        strcmp(scenario, "allocation") == 0 ? UMI_STATUS_OUT_OF_MEMORY :
        strcmp(scenario, "plan") == 0 ? UMI_STATUS_IO_ERROR :
        strcmp(scenario, "format") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT;
    CHECK(umi_repository_operation_service_execute(
        strcmp(scenario, "null") == 0 ? NULL : &request, result) == expected);
    CHECK(result->status == expected);
    if (expected == UMI_STATUS_OK) CHECK(result->issue_count == 3U && calls == 2U && context_valid);
    else {
        CHECK(result->issue_count == 0U && result->text[0] == '\0');
        unsigned expectedCalls = strcmp(scenario, "plan") == 0 ? 1U :
            strcmp(scenario, "format") == 0 ? 2U : 0U;
        CHECK(calls == expectedCalls);
        if (calls != 0U) CHECK(context_valid);
    }
    free(result);
    return 0;
}
