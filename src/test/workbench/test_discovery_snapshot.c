/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/test_discovery_snapshot.c
 *
 * PURPOSE:
 *   Implement test discovery snapshot state transitions and validation.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_discovery_snapshot.h"
#include <string.h>
UmiStatus umi_test_discovery_snapshot_init(UmiTestDiscoverySnapshot *model,const char *id,const char *label){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
UmiStatus umi_test_discovery_snapshot_set_active(UmiTestDiscoverySnapshot *model,bool active){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
UmiStatus umi_test_discovery_snapshot_set_count(UmiTestDiscoverySnapshot *model,uint32_t item_count){if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
UmiStatus umi_test_discovery_snapshot_set_state(UmiTestDiscoverySnapshot *model,UmiTestWorkbenchState state){UmiStatus s;if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);if(s==UMI_STATUS_OK)model->generation++;return s;}
int umi_test_discovery_snapshot_valid(const UmiTestDiscoverySnapshot *model){return model!=NULL&&umi_test_workbench_entry_valid(&model->value)&&model->item_count<=UMI_TEST_WORKBENCH_MAX_ITEMS&&model->generation>0U;}
