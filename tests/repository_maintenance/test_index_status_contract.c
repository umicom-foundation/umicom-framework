/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_index_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module index_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/index_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryIndexStatus s; umi_repository_index_status_init(&s); s.staged_paths=2U; s.staged_gitlinks=1U; assert(umi_repository_index_status_validate(&s)==UMI_STATUS_OK); return 0; }
