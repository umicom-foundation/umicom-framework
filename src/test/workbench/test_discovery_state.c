/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/test_discovery_state.c
 *
 * PURPOSE:
 *   Implement test discovery state state transitions and validation.
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
#include "umicom/test/workbench/test_discovery_state.h"
#include <string.h>
/*
 * Initialise test discovery state from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_test_discovery_state_init(UmiTestDiscoveryState *model,const char *id,const char *label){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
/*
 * Exercise test discovery state set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_discovery_state_set_active(UmiTestDiscoveryState *model,bool active){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by test discovery state set without changing
 * their state.
 */
UmiStatus umi_test_discovery_state_set_count(UmiTestDiscoveryState *model,uint32_t item_count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
/*
 * Exercise test discovery state set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_discovery_state_set_state(UmiTestDiscoveryState *model,UmiTestWorkbenchState state){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)model->generation++;return s;}
/*
 * Check that test discovery state satisfies its contract before another service relies on
 * it.
 */
int umi_test_discovery_state_valid(const UmiTestDiscoveryState *model){return model!=NULL&&umi_test_workbench_entry_valid(&model->value)&&model->item_count<=UMI_TEST_WORKBENCH_MAX_ITEMS&&model->generation>0U;}
