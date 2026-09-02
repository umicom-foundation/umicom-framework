/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/history.h
 *
 * PURPOSE:
 *   Own and parse bounded commit history records for source-control views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_HISTORY_H
#define UMICOM_VCS_HISTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs history data shared with callers of this public contract.
 */
typedef struct UmiVcsHistory UmiVcsHistory;

/**
 * Initialise vcs history from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_vcs_history_create(UmiVcsHistory **out_history);
/**
 * Release or reset state held by vcs history so the same storage can be reused safely.
 */
void umi_vcs_history_destroy(UmiVcsHistory *history);
/**
 * Release or reset state held by vcs history so the same storage can be reused safely.
 */
void umi_vcs_history_clear(UmiVcsHistory *history);
/**
 * Read vcs history into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_vcs_history_parse(UmiVcsHistory *history,
                                const char *output,
                                size_t *out_parsed);
/**
 * Return the number of records represented by vcs history without changing their state.
 */
size_t umi_vcs_history_count(const UmiVcsHistory *history);
/**
 * Find vcs history while leaving the underlying catalogue or model owned by this module.
 */
const UmiVcsCommit *umi_vcs_history_at(const UmiVcsHistory *history,
                                       size_t index);

#ifdef __cplusplus
}
#endif

#endif
