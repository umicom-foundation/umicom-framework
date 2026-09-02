/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/hub.h
 *
 * PURPOSE:
 *   Fan one diagnostic record out to several registered sinks.  Applications
 *   can send the same Framework diagnostic to stderr, a GTK pane, a journal,
 *   or a remote monitor without coupling Framework code to any UI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the diagnostic hub entry data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticHubEntry {
    UmiDiagnosticSink sink;
    void *user_data;
} UmiDiagnosticHubEntry;

/**
 * Represent the diagnostic hub data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticHub {
    UmiDiagnosticHubEntry entries[UMI_DIAGNOSTIC_HUB_MAX_SINKS];
    size_t count;
} UmiDiagnosticHub;

/**
 * Initialise diagnostic hub from caller-provided values so later operations receive a
 * known state.
 */
void umi_diagnostic_hub_init(UmiDiagnosticHub *hub);
/**
 * Add diagnostic hub only after its inputs and available capacity have been checked.
 */
UmiStatus umi_diagnostic_hub_add(UmiDiagnosticHub *hub,
                                 UmiDiagnosticSink sink,
                                 void *user_data);
/**
 * Remove diagnostic hub while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_diagnostic_hub_remove(UmiDiagnosticHub *hub,
                                    UmiDiagnosticSink sink,
                                    void *user_data);
/**
 * Return the number of records represented by diagnostic hub without changing their state.
 */
size_t umi_diagnostic_hub_count(const UmiDiagnosticHub *hub);
/**
 * Provide the diagnostic hub emit operation used by this module and its client
 * applications.
 */
void umi_diagnostic_hub_emit(const UmiDiagnostic *diagnostic, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
