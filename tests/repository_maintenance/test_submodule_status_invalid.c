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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/repository/submodule_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositorySubmoduleStatus s; umi_repository_submodule_status_init(&s); s.configured=1U; s.initialised=2U; assert(umi_repository_submodule_status_validate(&s)==UMI_STATUS_INVALID_STATE); return 0; }
