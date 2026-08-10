/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/status.h
 *
 * PURPOSE:
 *   Parse Git porcelain status records into provider-neutral change snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_STATUS_H
#define UMICOM_VCS_STATUS_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/change.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiVcsChangeState umi_vcs_status_code_state(char code);
UmiStatus umi_vcs_status_parse_porcelain(const char *output,
                                         UmiVcsChangeList *out_changes,
                                         size_t *out_parsed);

#ifdef __cplusplus
}
#endif

#endif
