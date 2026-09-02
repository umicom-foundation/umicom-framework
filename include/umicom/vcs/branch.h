/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/branch.h
 *
 * PURPOSE:
 *   Parse and retain branch identity, upstream and ahead/behind information.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_BRANCH_H
#define UMICOM_VCS_BRANCH_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the vcs branch parse status header operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_branch_parse_status_header(const char *line,
                                             UmiVcsBranch *out_branch);
/**
 * Provide the vcs branch parse current operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_branch_parse_current(const char *output,
                                       UmiVcsBranch *out_branch);
/**
 * Provide the vcs branch format operation used by this module and its client applications.
 */
UmiStatus umi_vcs_branch_format(const UmiVcsBranch *branch,
                                char *out_text,
                                size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
