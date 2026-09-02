/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_index_status_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module index_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/index_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryIndexStatus s; umi_repository_index_status_init(&s); s.staged_paths=0U; s.staged_gitlinks=1U; assert(umi_repository_index_status_validate(&s)==UMI_STATUS_INVALID_STATE); return 0; }
