#include "umicom/test/workbench/quality_workbench_snapshot.h"
#include <assert.h>
int main(void){UmiQualityWorkbenchSnapshot m;assert(umi_quality_workbench_snapshot_init(&m,"quality_workbench_snapshot","Quality Workbench Snapshot")==UMI_STATUS_OK);assert(umi_quality_workbench_snapshot_set_active(&m,true)==UMI_STATUS_OK);assert(umi_quality_workbench_snapshot_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_quality_workbench_snapshot_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_quality_workbench_snapshot_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
