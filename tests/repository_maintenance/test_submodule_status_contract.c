/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_submodule_status_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module submodule_status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/submodule_status.h"
int main(void){ UmiRepositorySubmoduleStatus s; umi_repository_submodule_status_init(&s); s.configured=2U; s.initialised=2U; assert(umi_repository_submodule_status_healthy(&s)); return 0; }
