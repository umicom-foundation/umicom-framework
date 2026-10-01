/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/fixture.h
 * PURPOSE: Provide a real canonical debugger workspace without starting a process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BREAKPOINT_EDIT_FIXTURE_H
#define UMICOM_BREAKPOINT_EDIT_FIXTURE_H
#include "umicom/debug/breakpoint_edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture {
    UmiProtocolTransport *transport; UmiProtocolClient *client; UmiDapClient dap;
    UmiDebugService *service; UmiDebugController *controller; UmiDebugWorkspace *workspace;
    UmiDebugBreakpointRegistry *registry;
} Fixture;
static inline UmiDebugBreakpointSnapshot Read(Fixture *f)
{
    UmiDebugBreakpointSnapshot value; OK(umi_debug_breakpoint_registry_find(f->registry, "first", &value)); return value;
}
static inline void Open(Fixture *f)
{
    OK(umi_protocol_transport_create_memory(16, &f->transport));
    OK(umi_protocol_client_create(f->transport, &f->client)); OK(umi_protocol_client_start(f->client));
    OK(umi_dap_client_init(&f->dap, f->client)); OK(umi_debug_service_create(&f->service));
    OK(umi_debug_controller_create(&f->dap, f->service, &f->controller));
    OK(umi_debug_workspace_create(f->service, f->controller, &f->workspace));
    f->registry = umi_debug_service_breakpoint(f->service);
    UmiDebugBreakpointSnapshot value = {0}; strcpy(value.id, "first"); strcpy(value.uri, "C:/source/main.c");
    value.line = 13; value.column = 1; value.enabled = 1; value.verified = 1;
    OK(umi_debug_breakpoint_registry_upsert(f->registry, &value));
}
static inline void Close(Fixture *f)
{
    umi_debug_workspace_destroy(f->workspace); umi_debug_controller_destroy(f->controller);
    umi_debug_service_destroy(f->service); umi_protocol_client_destroy(f->client);
    umi_protocol_transport_destroy(f->transport); memset(f, 0, sizeof *f);
}
#endif
