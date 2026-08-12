/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/application.h
 *
 * PURPOSE:
 *   Define an owned server-driven frontend application model combining widgets, signals, bindings, sessions, transports, render nodes and styles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiFrontendApplication UmiFrontendApplication;

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

UmiStatus umi_frontend_application_create(UmiFrontendApplication **out_service);
void umi_frontend_application_destroy(UmiFrontendApplication *service);
UmiStatus umi_frontend_application_snapshot(const UmiFrontendApplication *service, UmiFrontendApplicationSnapshot *out_snapshot);
UmiFrontendWidgetRegistry *umi_frontend_application_widgets(UmiFrontendApplication *service);
UmiFrontendSignalRegistry *umi_frontend_application_signals(UmiFrontendApplication *service);
UmiFrontendBindingRegistry *umi_frontend_application_bindings(UmiFrontendApplication *service);
UmiFrontendSessionRegistry *umi_frontend_application_sessions(UmiFrontendApplication *service);
UmiFrontendTransportRegistry *umi_frontend_application_transports(UmiFrontendApplication *service);
UmiFrontendRenderNodeRegistry *umi_frontend_application_render_nodes(UmiFrontendApplication *service);
UmiFrontendStyleRegistry *umi_frontend_application_styles(UmiFrontendApplication *service);

#ifdef __cplusplus
}
#endif
#endif
