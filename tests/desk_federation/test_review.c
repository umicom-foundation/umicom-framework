/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_review.c
 *
 * PURPOSE:
 *   Verify immutable review tokens, stale-plan rejection and honest partial outcomes.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

int TestReview(const char *name, const char *path)
{
    TestFixture *fixture = calloc(1U, sizeof *fixture);
    UmiDeskWorkspace workspace = TestWorkspace("finance", 3U);
    UmiDeskFederationPlan plan, other;
    UmiDeskFederationReport report;
    UmiDeskFederationActivity activity;
    (void)path;
    CHECK(fixture != NULL); OK(TestOpen(fixture, NULL));
    OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
    if (strcmp(name, "preview_inert") == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        CHECK(plan.count == 3U && plan.token != 0U && fixture->host.requestCount == 0U);
        CHECK(plan.items[0].action == UMI_DESK_FEDERATION_START);
    } else if (strcmp(name, "single_use") == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        OK(UmiDeskFederationExecute(fixture->service, plan.token, &report));
        CHECK(report.acceptedCount == 3U && fixture->host.requestCount == 3U);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(fixture->host.requestCount == 3U && report.count == 0U);
    } else if (strcmp(name, "cancel_inert") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        OK(UmiDeskFederationCancelPreview(fixture->service));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE && fixture->host.requestCount == 0U);
    } else if (strcmp(name, "new_preview_supersedes") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.accountant", &other));
        CHECK(other.token > plan.token);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE);
        OK(UmiDeskFederationExecute(fixture->service, other.token, &report)); CHECK(fixture->host.requestCount == 1U);
    } else if (strcmp(name, "caller_plan_cannot_tamper") == 0) {
        uint64_t token;
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan)); token = plan.token;
        strcpy(plan.items[0].app.id.value, "org.umicom.exchange"); plan.items[0].action = UMI_DESK_FEDERATION_ACTIVATE;
        OK(UmiDeskFederationExecute(fixture->service, token, &report));
        CHECK(strcmp(fixture->host.requests[0].value, "org.umicom.bank") == 0 && fixture->host.actions[0] == UMI_DESK_FEDERATION_START);
    } else if (strcmp(name, "running_activates") == 0) {
        fixture->host.catalogue.apps[0].running = true; fixture->host.catalogue.apps[0].state = UMI_DESK_FEDERATION_RUNNING;
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan)); CHECK(plan.items[0].action == UMI_DESK_FEDERATION_ACTIVATE);
        OK(UmiDeskFederationExecute(fixture->service, plan.token, &report)); CHECK(fixture->host.actions[0] == UMI_DESK_FEDERATION_ACTIVATE);
    } else if (strcmp(name, "skip_reasons") == 0) {
        UmiDeskFederationApp *app = &fixture->host.catalogue.apps[0];
        const UmiDeskFederationApp original = *app;
        for (size_t index = 0U; index < 6U; ++index) {
            UmiDeskFederationReason expected;
            *app = original;
            switch (index) {
            case 0U: app->installed = false; expected = UMI_DESK_FEDERATION_NOT_INSTALLED; break;
            case 1U: app->compatible = false; expected = UMI_DESK_FEDERATION_INCOMPATIBLE; break;
            case 2U: app->enabled = false; expected = UMI_DESK_FEDERATION_DISABLED; break;
            case 3U: app->state = UMI_DESK_FEDERATION_STARTING; expected = UMI_DESK_FEDERATION_TRANSITIONING; break;
            case 4U: app->state = UMI_DESK_FEDERATION_STOPPING; expected = UMI_DESK_FEDERATION_TRANSITIONING; break;
            default: app->state = UMI_DESK_FEDERATION_UNKNOWN; expected = UMI_DESK_FEDERATION_UNKNOWN_STATE; break;
            }
            OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
            CHECK(plan.items[0].reason == expected && plan.items[0].action == UMI_DESK_FEDERATION_SKIP);
            CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_UNAVAILABLE && report.skippedCount == 1U);
        }
        CHECK(fixture->host.requestCount == 0U);
    } else if (strcmp(name, "missing_application") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.missing", &plan));
        CHECK(plan.items[0].reason == UMI_DESK_FEDERATION_NOT_REGISTERED);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_UNAVAILABLE);
    } else if (strcmp(name, "self_skipped") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.desktop", &plan)); CHECK(plan.items[0].reason == UMI_DESK_FEDERATION_SELF);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_UNAVAILABLE && fixture->host.requestCount == 0U);
    } else if (strncmp(name, "stale_", 6U) == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        if (strcmp(name, "stale_generation") == 0) ++fixture->host.catalogue.apps[2].generation;
        else if (strcmp(name, "stale_token") == 0) ++fixture->host.catalogue.apps[2].processToken;
        else if (strcmp(name, "stale_presence") == 0) fixture->host.catalogue.apps[2].installed = false;
        else if (strcmp(name, "stale_running") == 0) fixture->host.catalogue.apps[2].running = true;
        else if (strcmp(name, "stale_new_registration") == 0) {
            OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.missing", &plan));
            strcpy(fixture->host.catalogue.apps[2].id.value, "org.umicom.missing");
        } else CHECK(false);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_BUSY);
        CHECK(fixture->host.requestCount == 0U && report.count == 0U);
    } else if (strcmp(name, "workspace_edit_cancels") == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        strcpy(workspace.title, "Changed"); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "partial_failure_continues") == 0) {
        fixture->host.requestStatus[0] = UMI_STATUS_PERMISSION_DENIED;
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(report.count == 3U && report.failedCount == 1U && report.acceptedCount == 2U && fixture->host.requestCount == 3U);
    } else if (strcmp(name, "mixed_skip_and_request") == 0) {
        fixture->host.catalogue.apps[1].enabled = false;
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_UNAVAILABLE);
        CHECK(report.acceptedCount == 2U && report.skippedCount == 1U && fixture->host.requestCount == 2U);
    } else if (strcmp(name, "per_item_recheck") == 0) {
        fixture->host.mutateSecond = true;
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_BUSY);
        CHECK(report.acceptedCount == 2U && report.failedCount == 1U);
    } else if (strcmp(name, "reentry_rejected") == 0) {
        fixture->host.reenter = true;
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        OK(UmiDeskFederationExecute(fixture->service, plan.token, &report)); CHECK(fixture->host.reentryStatus == UMI_STATUS_BUSY);
    } else if (strcmp(name, "acceptance_not_readiness") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        OK(UmiDeskFederationExecute(fixture->service, plan.token, &report));
        OK(UmiDeskFederationActivityAt(fixture->service, 0U, &activity));
        CHECK(activity.kind == UMI_DESK_ACTIVITY_REQUEST_ACCEPTED && activity.state == UMI_DESK_FEDERATION_STOPPED);
        CHECK(!fixture->host.catalogue.apps[0].running);
    } else if (strcmp(name, "read_failure_consumes") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        fixture->host.readStatus = UMI_STATUS_IO_ERROR;
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_IO_ERROR);
        fixture->host.readStatus = UMI_STATUS_OK;
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "bad_catalogue") == 0) {
        fixture->host.catalogue.apps[1] = fixture->host.catalogue.apps[0];
        CHECK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan) == UMI_STATUS_ALREADY_EXISTS);
        TestCatalogue(&fixture->host); fixture->host.catalogue.count = 65U;
        CHECK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan) == UMI_STATUS_CAPACITY_EXCEEDED);
        TestCatalogue(&fixture->host); memset(fixture->host.catalogue.apps[0].title, 'X', sizeof fixture->host.catalogue.apps[0].title);
        CHECK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "unrelated_change_allowed") == 0) {
        OK(UmiDeskFederationPrepareApplication(fixture->service, "org.umicom.bank", &plan));
        ++fixture->host.catalogue.apps[2].generation;
        OK(UmiDeskFederationExecute(fixture->service, plan.token, &report)); CHECK(report.acceptedCount == 1U);
    } else CHECK(false);
    TestClose(fixture); free(fixture); return 0;
}
