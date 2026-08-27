#include "umicom/test/workbench/test_discovery_snapshot.h"
#include <assert.h>
int main(void){UmiTestDiscoverySnapshot m;assert(umi_test_discovery_snapshot_init(&m,"test_discovery_snapshot","Test Discovery Snapshot")==UMI_STATUS_OK);assert(umi_test_discovery_snapshot_set_active(&m,true)==UMI_STATUS_OK);assert(umi_test_discovery_snapshot_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_test_discovery_snapshot_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_discovery_snapshot_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
