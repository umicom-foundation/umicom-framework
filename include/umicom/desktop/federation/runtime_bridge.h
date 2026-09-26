/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/federation/runtime_bridge.h
 *
 * PURPOSE:
 *   Bind reviewed workspace requests to the existing Desk runtime.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_RUNTIME_BRIDGE_H
#define UMICOM_DESKTOP_FEDERATION_RUNTIME_BRIDGE_H
#include "umicom/desktop/federation/service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeskRuntime UmiDeskRuntime;
/* The borrowed runtime remains the ONLY owner of process, catalogue, taskbar,
 * installation, readiness and launch policy. This bridge copies evidence and
 * dispatches through its public request function; it never spawns a process. */
UmiDeskFederationPorts UmiDeskFederationRuntimePorts(UmiDeskRuntime *runtime);
/* Save Desk's current picker selection, preserving order. Does not alter the
 * picker, launch programs or write any application-owned financial state. */
UmiStatus UmiDeskFederationSaveSelection(UmiDeskFederation *service,
    UmiDeskRuntime *runtime, const char *id, const char *title);
#ifdef __cplusplus
}
#endif
#endif
