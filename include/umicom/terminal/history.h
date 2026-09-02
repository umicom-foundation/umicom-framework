/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/history.h
 *
 * PURPOSE:
 *   Retain bounded command history with session, directory, exit-code and
 *   chronological navigation metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the terminal history entry data shared with callers of this public contract.
 */
typedef struct UmiTerminalHistoryEntry {
    uint64_t sequence;
    uint64_t timestamp_ns;
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char working_directory[UMI_TERMINAL_PATH_CAPACITY];
    char command[UMI_TERMINAL_COMMAND_CAPACITY];
    int exit_code;
    int completed;
} UmiTerminalHistoryEntry;

/**
 * Represent the terminal history stats data shared with callers of this public contract.
 */
typedef struct UmiTerminalHistoryStats {
    size_t capacity;
    size_t count;
    uint64_t appended;
    uint64_t succeeded;
    uint64_t failed;
    uint64_t revision;
} UmiTerminalHistoryStats;

/**
 * Represent the terminal history data shared with callers of this public contract.
 */
typedef struct UmiTerminalHistory UmiTerminalHistory;

/**
 * Initialise terminal history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_history_create(size_t capacity,
                                      UmiTerminalHistory **out_history);
/**
 * Release or reset state held by terminal history so the same storage can be reused
 * safely.
 */
void umi_terminal_history_destroy(UmiTerminalHistory *history);
/**
 * Add terminal history only after its inputs and available capacity have been checked.
 */
UmiStatus umi_terminal_history_append(UmiTerminalHistory *history,
                                      const UmiTerminalHistoryEntry *entry);
/**
 * Find terminal history while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_history_at(const UmiTerminalHistory *history,
                                  size_t index,
                                  UmiTerminalHistoryEntry *out_entry);
/**
 * Provide the terminal history search operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_history_search(const UmiTerminalHistory *history,
                                      const char *query,
                                      size_t start_index,
                                      size_t *out_index,
                                      UmiTerminalHistoryEntry *out_entry);
/**
 * Provide the terminal history previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_history_previous(const UmiTerminalHistory *history,
                                        size_t *cursor,
                                        UmiTerminalHistoryEntry *out_entry);
/**
 * Provide the terminal history next operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_history_next(const UmiTerminalHistory *history,
                                    size_t *cursor,
                                    UmiTerminalHistoryEntry *out_entry);
/**
 * Provide the terminal history stats operation used by this module and its client
 * applications.
 */
UmiTerminalHistoryStats umi_terminal_history_stats(
    const UmiTerminalHistory *history);
/**
 * Release or reset state held by terminal history so the same storage can be reused
 * safely.
 */
void umi_terminal_history_clear(UmiTerminalHistory *history);

#ifdef __cplusplus
}
#endif
#endif
