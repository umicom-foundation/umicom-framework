/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/history.h
 *
 * PURPOSE:
 *   Retain bounded command history with session, directory, exit-code and
 *   chronological navigation metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_HISTORY_H
#define UMICOM_TERMINAL_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalHistoryEntry {
    uint64_t sequence;
    uint64_t timestamp_ns;
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char working_directory[UMI_TERMINAL_PATH_CAPACITY];
    char command[UMI_TERMINAL_COMMAND_CAPACITY];
    int exit_code;
    int completed;
} UmiTerminalHistoryEntry;

typedef struct UmiTerminalHistoryStats {
    size_t capacity;
    size_t count;
    uint64_t appended;
    uint64_t succeeded;
    uint64_t failed;
    uint64_t revision;
} UmiTerminalHistoryStats;

typedef struct UmiTerminalHistory UmiTerminalHistory;

UmiStatus umi_terminal_history_create(size_t capacity,
                                      UmiTerminalHistory **out_history);
void umi_terminal_history_destroy(UmiTerminalHistory *history);
UmiStatus umi_terminal_history_append(UmiTerminalHistory *history,
                                      const UmiTerminalHistoryEntry *entry);
UmiStatus umi_terminal_history_at(const UmiTerminalHistory *history,
                                  size_t index,
                                  UmiTerminalHistoryEntry *out_entry);
UmiStatus umi_terminal_history_search(const UmiTerminalHistory *history,
                                      const char *query,
                                      size_t start_index,
                                      size_t *out_index,
                                      UmiTerminalHistoryEntry *out_entry);
UmiStatus umi_terminal_history_previous(const UmiTerminalHistory *history,
                                        size_t *cursor,
                                        UmiTerminalHistoryEntry *out_entry);
UmiStatus umi_terminal_history_next(const UmiTerminalHistory *history,
                                    size_t *cursor,
                                    UmiTerminalHistoryEntry *out_entry);
UmiTerminalHistoryStats umi_terminal_history_stats(
    const UmiTerminalHistory *history);
void umi_terminal_history_clear(UmiTerminalHistory *history);

#ifdef __cplusplus
}
#endif
#endif
