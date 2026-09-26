/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/federation/ports.h
 *
 * PURPOSE:
 *   Keep execution and runtime observation behind the existing host authority.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_FEDERATION_PORTS_H
#define UMICOM_DESKTOP_FEDERATION_PORTS_H
#include "umicom/desktop/federation/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Read returns a complete, bounded copied catalogue. It must not launch or
 * stop anything. Request rechecks the EXPECTED record immediately before
 * dispatch and returns BUSY when its state/identity changed. START must never
 * silently become ACTIVATE, nor ACTIVATE silently become START.
 * OK means the host accepted the request, NOT application readiness.
 * Callbacks execute synchronously on the owning thread and may not re-enter
 * or destroy the Federation service. The caller owns context until destroy. */
typedef struct UmiDeskFederationPorts {
    void *context;
    UmiStatus (*read)(void *context, UmiDeskFederationCatalogue *outCatalogue);
    UmiStatus (*request)(void *context, const UmiDeskFederationApp *expected,
        UmiDeskFederationAction action);
} UmiDeskFederationPorts;
#ifdef __cplusplus
}
#endif
#endif
