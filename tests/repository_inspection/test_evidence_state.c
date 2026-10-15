/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_evidence_state.c
 * PURPOSE: Keep unavailable and stale repository observations distinct from healthy findings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/inspection_service.h"
#include "umicom/repository/inspection_audit.h"
#include "umicom/repository/inspection_health.h"
#include "umicom/repository/inspection_text.h"
#include "umicom/repository/repository_operation_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

int main(int argc, char **argv)
{
    int failed = 0;
    UmiRepositoryRemediationService *owner = calloc(1U, sizeof *owner);
    UmiRepositoryOperationResult *result = calloc(1U, sizeof *result);
    CHECK(argc == 2 && owner != NULL && result != NULL);
    umi_repository_remediation_service_init(owner);
    UmiRepositoryInspectionService *service = &owner->inspection;
    const char *name = argv[1];
    UmiRepositoryInspectionEvidence expected = UMI_REPOSITORY_INSPECTION_CURRENT;
    int health = 0, ready = 1;
    /* Establish an accepted observation without launching Git. Each scenario
     * then changes one condition that the presentation must not overlook. */
    owner->revision = 3U;
    service->revision = service->snapshot.revision = 7U;
    if (strcmp(name, "empty") == 0) {
        service->revision = service->snapshot.revision = 0U;
        expected = UMI_REPOSITORY_INSPECTION_NONE; health = -1; ready = 0;
    } else if (strcmp(name, "initial_failure") == 0) {
        service->revision = service->snapshot.revision = 0U;
        service->last_status = UMI_STATUS_IO_ERROR;
        expected = UMI_REPOSITORY_INSPECTION_NONE; health = 2; ready = 0;
    } else if (strcmp(name, "failed") == 0) {
        service->last_status = UMI_STATUS_TIMEOUT;
        expected = UMI_REPOSITORY_INSPECTION_STALE; health = 2; ready = 0;
    } else if (strcmp(name, "mismatch") == 0) {
        service->snapshot.revision = 6U;
        expected = UMI_REPOSITORY_INSPECTION_STALE; health = 2; ready = 0;
    } else if (strcmp(name, "warning") == 0) {
        service->report.warnings = 1U; health = 1;
    } else if (strcmp(name, "error") == 0) {
        service->maintenance.report.errors = 1U; health = 2; ready = 0;
    } else if (strcmp(name, "plan_failed") == 0) {
        owner->last_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "plan_empty") == 0) {
        owner->revision = 0U;
    } else if (strcmp(name, "null") == 0) {
        CHECK(UmiRepositoryInspectionEvidenceRead(NULL) == UMI_REPOSITORY_INSPECTION_NONE);
        CHECK(!umi_repository_inspection_audit_ready(NULL));
        CHECK(umi_repository_inspection_health_code(NULL) == -1);
        CHECK(!UmiRepositoryRemediationEvidenceCurrent(NULL));
        CHECK(umi_repository_inspection_text_format(NULL, result->text, sizeof result->text) ==
            UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    } else CHECK(strcmp(name, "current") == 0 || strcmp(name, "small_output") == 0);
    CHECK(UmiRepositoryInspectionEvidenceRead(service) == expected);
    CHECK(umi_repository_inspection_health_code(service) == health);
    CHECK(umi_repository_inspection_audit_ready(service) == ready);
    if (strcmp(name, "small_output") == 0) {
        char tiny[4] = {0};
        CHECK(umi_repository_inspection_text_format(service, tiny, sizeof tiny) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(tiny[sizeof tiny - 1U] == '\0');
        goto cleanup;
    }
    CHECK(umi_repository_inspection_text_format(service, result->text, sizeof result->text) ==
        UMI_STATUS_OK);
    CHECK(strstr(result->text, UmiRepositoryInspectionEvidenceText(expected)) != NULL);
    CHECK(strstr(result->text, umi_status_text(service->last_status)) != NULL);
    CHECK(strstr(result->text, "Accepted revision: ") != NULL);
    int usable = expected == UMI_REPOSITORY_INSPECTION_CURRENT &&
        owner->last_status == UMI_STATUS_OK && owner->revision != 0U;
    CHECK(UmiRepositoryRemediationEvidenceCurrent(owner) == usable);
    UmiStatus formatted = umi_repository_operation_formatter_format(
        UMI_REPOSITORY_OPERATION_STATUS, owner, result);
    if (usable) CHECK(formatted == UMI_STATUS_OK);
    else CHECK(formatted != UMI_STATUS_OK && result->text[0] == '\0' &&
        result->issue_count == 0U && result->action_count == 0U);
cleanup:
    free(result); free(owner);
    return failed;
}
