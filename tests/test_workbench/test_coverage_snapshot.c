/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_coverage_snapshot.c
 *
 * PURPOSE:
 *   Implement the test coverage snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/coverage_snapshot.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCoverageSnapshot m;assert(umi_coverage_snapshot_init(&m,"coverage_snapshot","Coverage Snapshot")==UMI_STATUS_OK);assert(umi_coverage_snapshot_set_active(&m,true)==UMI_STATUS_OK);assert(umi_coverage_snapshot_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_coverage_snapshot_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_coverage_snapshot_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
