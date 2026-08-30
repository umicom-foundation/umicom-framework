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

typedef struct UmiVcsHistory UmiVcsHistory;

UmiStatus umi_vcs_history_create(UmiVcsHistory **out_history);
void umi_vcs_history_destroy(UmiVcsHistory *history);
void umi_vcs_history_clear(UmiVcsHistory *history);
UmiStatus umi_vcs_history_parse(UmiVcsHistory *history,
                                const char *output,
                                size_t *out_parsed);
size_t umi_vcs_history_count(const UmiVcsHistory *history);
const UmiVcsCommit *umi_vcs_history_at(const UmiVcsHistory *history,
                                       size_t index);

#ifdef __cplusplus
}
#endif

#endif
