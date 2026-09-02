/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_worktree_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module worktree_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/worktree_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositoryWorktreeStatus s; umi_repository_worktree_status_init(&s); s.modified=2U; assert(umi_repository_worktree_status_dirty(&s)); assert(umi_repository_worktree_status_change_count(&s)==2U); return 0; }
