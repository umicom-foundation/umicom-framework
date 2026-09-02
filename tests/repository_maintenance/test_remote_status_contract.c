/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_remote_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module remote_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/remote_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryRemoteStatus s; umi_repository_remote_status_init(&s); s.remote_count=1U; s.has_origin=1; assert(umi_repository_remote_status_validate(&s)==UMI_STATUS_OK); return 0; }
