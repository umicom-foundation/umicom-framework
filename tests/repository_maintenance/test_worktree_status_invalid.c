/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_worktree_status_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module worktree_status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/worktree_status.h"
int main(void){ assert(!umi_repository_worktree_status_dirty(NULL)); return 0; }
