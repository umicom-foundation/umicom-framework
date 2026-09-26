/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_support.c
 *
 * PURPOSE:
 *   Model only the host port, never a successful operating-system launch.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

UmiStatus TestRead(void *context, UmiDeskFederationCatalogue *outCatalogue)
{
    TestHost *host = context;
    ++host->readCount;
    if (host->readStatus != UMI_STATUS_OK) return host->readStatus;
    *outCatalogue = host->catalogue; return UMI_STATUS_OK;
}
UmiStatus TestRequest(void *context, const UmiDeskFederationApp *expected, UmiDeskFederationAction action)
{
    TestHost *host = context;
    const UmiDeskFederationApp *current = NULL;
    size_t call = host->requestCount++;
    if (call >= UMI_DESK_FEDERATION_MAX_APPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < host->catalogue.count; ++index)
        if (strcmp(host->catalogue.apps[index].id.value, expected->id.value) == 0) current = &host->catalogue.apps[index];
    host->requests[call] = expected->id; host->actions[call] = action;
    if (current == NULL) return UMI_STATUS_NOT_FOUND;
    /* Independent port-side comparison, not the production comparison helper. */
    if (current->generation != expected->generation || current->processToken != expected->processToken ||
        current->state != expected->state || current->enabled != expected->enabled || current->installed != expected->installed ||
        current->compatible != expected->compatible || current->running != expected->running) return UMI_STATUS_BUSY;
    if ((action == UMI_DESK_FEDERATION_START && current->running) ||
        (action == UMI_DESK_FEDERATION_ACTIVATE && !current->running)) return UMI_STATUS_BUSY;
    if (host->reenter) host->reentryStatus = UmiDeskFederationCancelPreview(host->service);
    if (host->mutateSecond && call == 0U) ++host->catalogue.apps[1].generation;
    return host->requestStatus[call];
}
void TestCatalogue(TestHost *host)
{
    static const char *const ids[] = {"org.umicom.bank", "org.umicom.accountant", "org.umicom.exchange", "org.umicom.desktop"};
    static const char *const titles[] = {"Umicom Bank", "Umicom Accountant", "Umicom Commodity Exchange", "Umicom Desk"};
    memset(host, 0, sizeof *host); host->catalogue.count = 4U;
    for (size_t index = 0U; index < 4U; ++index) {
        UmiDeskFederationApp *app = &host->catalogue.apps[index];
        (void)snprintf(app->id.value, sizeof app->id.value, "%s", ids[index]);
        (void)snprintf(app->title, sizeof app->title, "%s", titles[index]);
        app->installed = true; app->compatible = true; app->enabled = true;
        app->state = UMI_DESK_FEDERATION_STOPPED; app->generation = 1U;
    }
    host->catalogue.apps[3].running = true;
    host->catalogue.apps[3].state = UMI_DESK_FEDERATION_RUNNING;
}
UmiStatus TestOpen(TestFixture *fixture, const char *path)
{
    UmiStatus status;
    UmiDeskFederationPorts ports;
    memset(fixture, 0, sizeof *fixture); TestCatalogue(&fixture->host);
    status = path == NULL ? umi_data_server_create_memory(&fixture->server) : umi_data_server_create_sqlite(path, &fixture->server);
    if (status != UMI_STATUS_OK) return status;
    ports = (UmiDeskFederationPorts){&fixture->host, TestRead, TestRequest};
    status = UmiDeskFederationCreate(fixture->server, &ports, "org.umicom.desktop", &fixture->service);
    fixture->host.service = fixture->service;
    if (status == UMI_STATUS_OK) status = UmiDeskFederationReload(fixture->service);
    return status;
}
void TestClose(TestFixture *fixture)
{
    UmiDeskFederationDestroy(fixture->service); umi_data_server_destroy(fixture->server);
    fixture->service = NULL; fixture->server = NULL;
}
UmiDeskWorkspace TestWorkspace(const char *id, size_t members)
{
    UmiDeskWorkspace workspace = {0};
    static const char *const ids[] = {"org.umicom.bank", "org.umicom.accountant", "org.umicom.exchange"};
    workspace.kind = UMI_DESK_WORKSPACE_GROUP;
    (void)snprintf(workspace.id, sizeof workspace.id, "%s", id);
    (void)snprintf(workspace.title, sizeof workspace.title, "%s", "Finance work");
    workspace.memberCount = members;
    for (size_t index = 0U; index < members && index < UMI_DESK_FEDERATION_MAX_APPS; ++index) {
        if (index < 3U) (void)snprintf(workspace.members[index].value, sizeof workspace.members[index].value, "%s", ids[index]);
        else (void)snprintf(workspace.members[index].value, sizeof workspace.members[index].value, "org.umicom.practice.%zu", index);
    }
    return workspace;
}
uint64_t TestRevision(UmiDeskFederation *service)
{
    UmiDeskFederationSnapshot snapshot;
    return UmiDeskFederationSnapshotGet(service, &snapshot) == UMI_STATUS_OK ? snapshot.revision : UINT64_MAX;
}
