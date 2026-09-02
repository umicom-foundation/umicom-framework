/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/commit.h
 *
 * PURPOSE:
 *   Parse version-control commit records from a delimiter-safe Git format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_COMMIT_H
#define UMICOM_VCS_COMMIT_H

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the vcs commit parse record operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_commit_parse_record(const char *record,
                                      UmiVcsCommit *out_commit);

#ifdef __cplusplus
}
#endif

#endif
