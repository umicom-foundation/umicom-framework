/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/application.h
 *
 * PURPOSE:
 *   Define an owned server-driven frontend application model combining widgets, signals, bindings, sessions, transports, render nodes and styles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_FRONTEND_APPLICATION_H
#define UMICOM_FRONTEND_APPLICATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/frontend/widget_tree.h"
#include "umicom/frontend/signal.h"
#include "umicom/frontend/binding.h"
#include "umicom/frontend/web_session.h"
#include "umicom/frontend/transport.h"
#include "umicom/frontend/render_tree.h"
#include "umicom/frontend/web_style.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the frontend application data shared with callers of this public contract.
 */
typedef struct UmiFrontendApplication UmiFrontendApplication;

/**
 * Represent the frontend application snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendApplicationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t widgets_count;
    size_t signals_count;
    size_t bindings_count;
    size_t sessions_count;
    size_t transports_count;
    size_t render_nodes_count;
    size_t styles_count;
    uint64_t revision;
} UmiFrontendApplicationSnapshot;

/**
 * Initialise frontend application from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_frontend_application_create(UmiFrontendApplication **out_service);
/**
 * Release or reset state held by frontend application so the same storage can be reused
 * safely.
 */
void umi_frontend_application_destroy(UmiFrontendApplication *service);
/**
 * Provide the frontend application snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_application_snapshot(const UmiFrontendApplication *service, UmiFrontendApplicationSnapshot *out_snapshot);
/**
 * Provide the frontend application widgets operation used by this module and its client
 * applications.
 */
UmiFrontendWidgetRegistry *umi_frontend_application_widgets(UmiFrontendApplication *service);
/**
 * Provide the frontend application signals operation used by this module and its client
 * applications.
 */
UmiFrontendSignalRegistry *umi_frontend_application_signals(UmiFrontendApplication *service);
/**
 * Provide the frontend application bindings operation used by this module and its client
 * applications.
 */
UmiFrontendBindingRegistry *umi_frontend_application_bindings(UmiFrontendApplication *service);
/**
 * Provide the frontend application sessions operation used by this module and its client
 * applications.
 */
UmiFrontendSessionRegistry *umi_frontend_application_sessions(UmiFrontendApplication *service);
/**
 * Provide the frontend application transports operation used by this module and its client
 * applications.
 */
UmiFrontendTransportRegistry *umi_frontend_application_transports(UmiFrontendApplication *service);
/**
 * Provide the frontend application render nodes operation used by this module and its
 * client applications.
 */
UmiFrontendRenderNodeRegistry *umi_frontend_application_render_nodes(UmiFrontendApplication *service);
/**
 * Provide the frontend application styles operation used by this module and its client
 * applications.
 */
UmiFrontendStyleRegistry *umi_frontend_application_styles(UmiFrontendApplication *service);

#ifdef __cplusplus
}
#endif
#endif
