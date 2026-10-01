/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/fixture.h
 * PURPOSE: Provide a real canonical debugger workspace without starting a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WATCH_EDIT_FIXTURE_H
#define UMICOM_WATCH_EDIT_FIXTURE_H
#include "umicom/debug/watch_edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture {
    UmiProtocolTransport *transport; UmiProtocolClient *client; UmiDapClient dap;
    UmiDebugService *service; UmiDebugController *controller; UmiDebugWorkspace *workspace;
    UmiDebugWatchRegistry *registry;
    int ownsService;
} Fixture;
static inline UmiDebugWatchSnapshot Read(Fixture *f)
{
    UmiDebugWatchSnapshot value; OK(umi_debug_watch_registry_find(f->registry, "first", &value)); return value;
}
static inline void OpenWithService(Fixture *f, UmiDebugService *service)
{
    OK(umi_protocol_transport_create_memory(16, &f->transport));
    OK(umi_protocol_client_create(f->transport, &f->client)); OK(umi_protocol_client_start(f->client));
    OK(umi_dap_client_init(&f->dap, f->client));
    f->service = service;
    if (service == NULL) { OK(umi_debug_service_create(&f->service)); f->ownsService = 1; }
    OK(umi_debug_controller_create(&f->dap, f->service, &f->controller));
    OK(umi_debug_workspace_create(f->service, f->controller, &f->workspace));
    f->registry = umi_debug_service_watch(f->service);
    UmiDebugWatchSnapshot value = {0}; strcpy(value.id, "first"); strcpy(value.expression, "savedNotes");
    strcpy(value.value, "old"); strcpy(value.type, "int"); strcpy(value.session_id, "previous");
    value.enabled = 1; value.valid = 1;
    OK(umi_debug_watch_registry_upsert(f->registry, &value));
}
static inline void Open(Fixture *f) { OpenWithService(f, NULL); }
static inline void Close(Fixture *f)
{
    umi_debug_workspace_destroy(f->workspace); umi_debug_controller_destroy(f->controller);
    if (f->ownsService) umi_debug_service_destroy(f->service);
    umi_protocol_client_destroy(f->client);
    umi_protocol_transport_destroy(f->transport); memset(f, 0, sizeof *f);
}
#endif
