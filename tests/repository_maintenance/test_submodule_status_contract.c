/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_submodule_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module submodule_status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/submodule_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiRepositorySubmoduleStatus s; umi_repository_submodule_status_init(&s); s.configured=2U; s.initialised=2U; assert(umi_repository_submodule_status_healthy(&s)); return 0; }
