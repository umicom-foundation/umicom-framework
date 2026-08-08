/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/hub.h
 *
 * PURPOSE:
 *   Fan one diagnostic record out to several registered sinks.  Applications
 *   can send the same Framework diagnostic to stderr, a GTK pane, a journal,
 *   or a remote monitor without coupling Framework code to any UI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_HUB_H
#define UMICOM_DIAGNOSTICS_HUB_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_HUB_MAX_SINKS 8U

typedef struct UmiDiagnosticHubEntry {
    UmiDiagnosticSink sink;
    void *user_data;
} UmiDiagnosticHubEntry;

typedef struct UmiDiagnosticHub {
    UmiDiagnosticHubEntry entries[UMI_DIAGNOSTIC_HUB_MAX_SINKS];
    size_t count;
} UmiDiagnosticHub;

void umi_diagnostic_hub_init(UmiDiagnosticHub *hub);
UmiStatus umi_diagnostic_hub_add(UmiDiagnosticHub *hub,
                                 UmiDiagnosticSink sink,
                                 void *user_data);
UmiStatus umi_diagnostic_hub_remove(UmiDiagnosticHub *hub,
                                    UmiDiagnosticSink sink,
                                    void *user_data);
size_t umi_diagnostic_hub_count(const UmiDiagnosticHub *hub);
void umi_diagnostic_hub_emit(const UmiDiagnostic *diagnostic, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
