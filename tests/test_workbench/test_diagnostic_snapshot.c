#include "umicom/test/workbench/diagnostic_snapshot.h"
#include <assert.h>
int main(void){UmiDiagnosticSnapshot m;assert(umi_diagnostic_snapshot_init(&m,"diagnostic_snapshot","Diagnostic Snapshot")==UMI_STATUS_OK);assert(umi_diagnostic_snapshot_set_active(&m,true)==UMI_STATUS_OK);assert(umi_diagnostic_snapshot_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_diagnostic_snapshot_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_diagnostic_snapshot_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
