/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_setup/test_review.c
 * PURPOSE: Refuse stale setup reviews and replace both collections without retaining runtime results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    Fixture f = {0};
    Open(&f);
    SeedWatch(&f);
    UmiDebugSetup *setup = Setup();
    UmiDebugSetupReview *review = NULL;
    OK(UmiDebugSetupReviewCreate(f.workspace, setup, &review));
    UmiDebugWatchRegistry *watches = umi_debug_service_watch(f.service);
    UmiDebugBreakpointRegistry *points = f.registry;
    uint64_t watchRevision = umi_debug_watch_registry_revision(watches),
             pointRevision = umi_debug_breakpoint_registry_revision(points);
    if (strcmp(mode, "apply") == 0 || strcmp(mode, "reverse") == 0 || strcmp(mode, "empty") == 0)
    {
        if (strcmp(mode, "empty") == 0)
        {
            UmiDebugSetupReviewDestroy(review);
            review = NULL;
            UmiDebugSetupDestroy(setup);
            setup = NULL;
            OK(UmiDebugSetupCreate("Empty deliberately", &setup));
            OK(UmiDebugSetupReviewCreate(f.workspace, setup, &review));
        }
        OK(UmiDebugSetupReviewApply(f.workspace, review));
        CHECK(umi_debug_service_watch(f.service) == watches &&
              umi_debug_service_breakpoint(f.service) == points);
        CHECK(umi_debug_watch_registry_revision(watches) == watchRevision + 1U &&
              umi_debug_breakpoint_registry_revision(points) == pointRevision + 1U);
        CHECK(UmiDebugSetupReviewApply(f.workspace, review) == UMI_STATUS_BUSY);
        if (strcmp(mode, "empty") == 0)
            CHECK(umi_debug_watch_registry_count(watches) == 0U &&
                  umi_debug_breakpoint_registry_count(points) == 0U);
        else
        {
            UmiDebugWatchSnapshot watch;
            UmiDebugBreakpointSnapshot point;
            OK(umi_debug_watch_registry_at(watches, 0U, &watch));
            OK(umi_debug_breakpoint_registry_at(points, 0U, &point));
            CHECK(!watch.valid && watch.value[0] == '\0' && watch.type[0] == '\0' &&
                  watch.session_id[0] == '\0');
            CHECK(strcmp(watch.expression, "items[3].price") == 0 && point.line == 29U && !point.verified &&
                  point.session_id[0] == '\0');
            CHECK(!point.enabled && strcmp(point.condition, "count > 8") == 0 &&
                  strcmp(point.log_message, "count={count}") == 0);
            /* New ordinary watches must not overwrite restored IDs. */
            OK(umi_debug_workspace_add_watch(f.workspace, "new watch", NULL, 0U));
            CHECK(umi_debug_watch_registry_count(watches) == 2U);
        }
        if (strcmp(mode, "reverse") == 0)
        {
            UmiDebugSetupReview *reverse = NULL;
            OK(UmiDebugSetupReviewCreate(f.workspace, UmiDebugSetupReviewBefore(review), &reverse));
            OK(UmiDebugSetupReviewApply(f.workspace, reverse));
            UmiDebugBreakpointSnapshot point;
            UmiDebugWatchSnapshot watch;
            OK(umi_debug_breakpoint_registry_at(points, 0U, &point));
            OK(umi_debug_watch_registry_at(watches, 0U, &watch));
            CHECK(point.line == 13U && strcmp(watch.expression, "count") == 0 && !watch.valid &&
                  !point.verified);
            UmiDebugSetupReviewDestroy(reverse);
        }
    }
    else if (strcmp(mode, "lifetime") == 0)
    {
        Close(&f);
        UmiDebugSetupSummary summary;
        OK(UmiDebugSetupInspect(UmiDebugSetupReviewBefore(review), &summary));
        CHECK(summary.breakpoints == 1U);
        OK(UmiDebugSetupInspect(UmiDebugSetupReviewAfter(review), &summary));
        CHECK(summary.watches == 1U);
    }
    else if (strcmp(mode, "unrelated") == 0)
    {
        UmiDebugConsoleEntrySnapshot entry = {0};
        strcpy(entry.id, "output");
        OK(umi_debug_console_entry_registry_upsert(umi_debug_service_console_entry(f.service), &entry));
        OK(UmiDebugSetupReviewApply(f.workspace, review));
    }
    else
    {
        if (strcmp(mode, "watch-stale") == 0)
            SeedWatch(&f);
        else if (strcmp(mode, "breakpoint-stale") == 0)
        {
            UmiDebugBreakpointSnapshot point = Read(&f);
            point.line++;
            OK(umi_debug_breakpoint_registry_upsert(points, &point));
        }
        else if (strcmp(mode, "owner-stale") == 0)
        {
            umi_debug_workspace_destroy(f.workspace);
            OK(umi_debug_workspace_create(f.service, f.controller, &f.workspace));
        }
        else if (strcmp(mode, "configuration-stale") == 0)
        {
            UmiDebugLaunchConfigurationSnapshot item = {0};
            strcpy(item.id, "new");
            OK(umi_debug_launch_configuration_registry_upsert(
                umi_debug_service_launch_configuration(f.service), &item));
        }
        else if (strcmp(mode, "session-stale") == 0)
        {
            UmiDebugSessionSnapshot item = {0};
            strcpy(item.id, "new");
            OK(umi_debug_session_registry_upsert(umi_debug_service_session(f.service), &item));
        }
        else if (strcmp(mode, "controller-stale") == 0 || strcmp(mode, "active") == 0)
        {
            OK(umi_debug_controller_initialize(f.controller, "fixture"));
            if (strcmp(mode, "active") == 0)
            {
                UmiDebugSetupReviewDestroy(review);
                review = NULL;
                OK(UmiDebugSetupReviewCreate(f.workspace, setup, &review));
            }
        }
        else
            return 2;
        watchRevision = umi_debug_watch_registry_revision(watches);
        pointRevision = umi_debug_breakpoint_registry_revision(points);
        CHECK(UmiDebugSetupReviewApply(f.workspace, review) == UMI_STATUS_BUSY);
        CHECK(watchRevision == umi_debug_watch_registry_revision(watches) &&
              pointRevision == umi_debug_breakpoint_registry_revision(points));
    }
    UmiDebugSetupReviewDestroy(review);
    UmiDebugSetupDestroy(setup);
    Close(&f);
    return 0;
}
