/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/types.h
 *
 * PURPOSE:
 *   Define bounded view limits and snapshots for reusable terminal, process and
 *   task workbench presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_TYPES_H
#define UMICOM_TERMINAL_UI_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/build/graph.h"
#include "umicom/terminal/terminal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TERMINAL_UI_TRANSCRIPT_ROWS 256U
#define UMI_TERMINAL_UI_PROCESS_ROWS 64U
#define UMI_TERMINAL_UI_TASK_ROWS 128U

typedef struct UmiTerminalControllerSnapshot {
    size_t profiles;
    size_t sessions;
    size_t tabs;
    size_t split_nodes;
    size_t history_entries;
    size_t events;
    char active_session_id[UMI_TERMINAL_ID_CAPACITY];
    char default_profile_id[UMI_TERMINAL_ID_CAPACITY];
    UmiTerminalMetrics metrics;
    UmiBuildGraphSnapshot operation_graph;
    uint64_t revision;
} UmiTerminalControllerSnapshot;

#ifdef __cplusplus
}
#endif
#endif
