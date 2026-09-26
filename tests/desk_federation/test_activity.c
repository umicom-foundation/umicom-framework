/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_activity.c
 *
 * PURPOSE:
 *   Exercise honest observation baselines, bounded activity and acknowledgement.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
int TestActivity(const char *name, const char *path)
{
    TestFixture *fixture = calloc(1U, sizeof *fixture);
    UmiDeskFederationSnapshot snapshot;
    UmiDeskFederationActivity activity;
    (void)path;
    CHECK(fixture != NULL); OK(TestOpen(fixture, NULL)); OK(UmiDeskFederationPoll(fixture->service));
    if (strcmp(name, "initial_baseline") == 0) {
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 0U && fixture->host.requestCount == 0U);
    } else if (strcmp(name, "unchanged_deduplicated") == 0) {
        for (size_t index = 0U; index < 30U; ++index) OK(UmiDeskFederationPoll(fixture->service));
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 0U);
    } else if (strcmp(name, "state_change") == 0) {
        fixture->host.catalogue.apps[0].state = UMI_DESK_FEDERATION_FAILED;
        OK(UmiDeskFederationPoll(fixture->service)); OK(UmiDeskFederationActivityAt(fixture->service, 0U, &activity));
        CHECK(activity.kind == UMI_DESK_ACTIVITY_OBSERVED && activity.state == UMI_DESK_FEDERATION_FAILED && !activity.read);
        OK(UmiDeskFederationPoll(fixture->service)); OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 1U);
    } else if (strcmp(name, "acknowledge") == 0) {
        ++fixture->host.catalogue.apps[0].generation; OK(UmiDeskFederationPoll(fixture->service));
        OK(UmiDeskFederationAcknowledgeAll(fixture->service)); OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot));
        CHECK(snapshot.unreadCount == 0U && snapshot.activityCount == 1U);
        ++fixture->host.catalogue.apps[1].generation; OK(UmiDeskFederationPoll(fixture->service));
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.unreadCount == 1U && snapshot.activityCount == 2U);
    } else if (strcmp(name, "bounded_ring") == 0) {
        for (size_t index = 0U; index < 80U; ++index) { ++fixture->host.catalogue.apps[0].generation; OK(UmiDeskFederationPoll(fixture->service)); }
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 64U && snapshot.droppedActivity == 16U);
        OK(UmiDeskFederationActivityAt(fixture->service, 0U, &activity)); CHECK(activity.sequence == 80U);
        OK(UmiDeskFederationActivityAt(fixture->service, 63U, &activity)); CHECK(activity.sequence == 17U);
        CHECK(UmiDeskFederationActivityAt(fixture->service, 64U, &activity) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "read_failure_preserves_baseline") == 0) {
        fixture->host.readStatus = UMI_STATUS_IO_ERROR; ++fixture->host.catalogue.apps[0].generation;
        CHECK(UmiDeskFederationPoll(fixture->service) == UMI_STATUS_IO_ERROR);
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 0U);
        fixture->host.readStatus = UMI_STATUS_OK; OK(UmiDeskFederationPoll(fixture->service));
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 1U);
    } else if (strcmp(name, "removed_registration") == 0) {
        fixture->host.catalogue.apps[0] = fixture->host.catalogue.apps[3]; fixture->host.catalogue.count = 3U;
        OK(UmiDeskFederationPoll(fixture->service)); OK(UmiDeskFederationActivityAt(fixture->service, 0U, &activity));
        CHECK(activity.status == UMI_STATUS_NOT_FOUND && activity.state == UMI_DESK_FEDERATION_UNKNOWN);
    } else if (strcmp(name, "non_durable") == 0) {
        UmiDeskFederationPorts ports = {&fixture->host, TestRead, TestRequest};
        ++fixture->host.catalogue.apps[0].generation; OK(UmiDeskFederationPoll(fixture->service));
        CHECK(umi_data_server_count(fixture->server) == 0U);
        UmiDeskFederationDestroy(fixture->service);
        OK(UmiDeskFederationCreate(fixture->server, &ports, "org.umicom.desktop", &fixture->service));
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot)); CHECK(snapshot.activityCount == 0U);
    } else CHECK(false);
    TestClose(fixture); free(fixture); return 0;
}
