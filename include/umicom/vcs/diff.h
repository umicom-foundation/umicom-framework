/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/diff.h
 *
 * PURPOSE:
 *   Parse Git numstat output and aggregate insertion and deletion totals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_DIFF_H
#define UMICOM_VCS_DIFF_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the vcs diff parse numstat operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_diff_parse_numstat(const char *output,
                                     UmiVcsDiffStat *out_stats,
                                     size_t capacity,
                                     size_t *out_count);
/**
 * Provide the vcs diff totals operation used by this module and its client applications.
 */
void umi_vcs_diff_totals(const UmiVcsDiffStat *stats,
                         size_t count,
                         size_t *out_insertions,
                         size_t *out_deletions,
                         size_t *out_binary_files);

#ifdef __cplusplus
}
#endif

#endif
