/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_submodule_status_invalid.c
 *
 * PURPOSE:
 *   Verify invalid-state handling for repository maintenance module submodule_status.
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
#include "umicom/repository/submodule_status.h"
int main(void){ UmiRepositorySubmoduleStatus s; umi_repository_submodule_status_init(&s); s.configured=1U; s.initialised=2U; assert(umi_repository_submodule_status_validate(&s)==UMI_STATUS_INVALID_STATE); return 0; }
