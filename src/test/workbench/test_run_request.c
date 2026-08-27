/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/test_run_request.c
 *
 * PURPOSE:
 *   Implement test run request state transitions and validation.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/test_run_request.h"
#include <string.h>
UmiStatus umi_test_run_request_init(UmiTestRunRequest *model,const char *id,const char *label){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);if(umi_test_workbench_entry_init(&model->value,id,label)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->generation=1U;return UMI_STATUS_OK;}
UmiStatus umi_test_run_request_set_active(UmiTestRunRequest *model,bool active){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->active=active;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
UmiStatus umi_test_run_request_set_count(UmiTestRunRequest *model,uint32_t item_count){if(model==NULL||item_count>UMI_TEST_WORKBENCH_MAX_ITEMS)return UMI_STATUS_INVALID_ARGUMENT;model->item_count=item_count;model->generation++;model->value.revision++;return UMI_STATUS_OK;}
UmiStatus umi_test_run_request_set_state(UmiTestRunRequest *model,UmiTestWorkbenchState state){UmiStatus s;if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_entry_set_state(&model->value,state);if(s==UMI_STATUS_OK)model->generation++;return s;}
int umi_test_run_request_valid(const UmiTestRunRequest *model){return model!=NULL&&umi_test_workbench_entry_valid(&model->value)&&model->item_count<=UMI_TEST_WORKBENCH_MAX_ITEMS&&model->generation>0U;}
