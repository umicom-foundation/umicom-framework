/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/git_cli.h
 *
 * PURPOSE:
 *   Create a Git command-line provider behind the stable Framework VCS contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_GIT_CLI_H
#define UMICOM_VCS_GIT_CLI_H

#include "umicom/base/status.h"
#include "umicom/vcs/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_vcs_git_cli_provider(UmiVcsProvider *out_provider);
UmiStatus umi_vcs_git_cli_is_repository(const char *root, int *out_is_repo);

#ifdef __cplusplus
}
#endif

#endif
