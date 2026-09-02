/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/status.h
 *
 * PURPOSE:
 *   Parse Git porcelain status records into provider-neutral change snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_STATUS_H
#define UMICOM_VCS_STATUS_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/change.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the vcs status code state operation used by this module and its client
 * applications.
 */
UmiVcsChangeState umi_vcs_status_code_state(char code);
/**
 * Provide the vcs status parse porcelain operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_status_parse_porcelain(const char *output,
                                         UmiVcsChangeList *out_changes,
                                         size_t *out_parsed);

#ifdef __cplusplus
}
#endif

#endif
