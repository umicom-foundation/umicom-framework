/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test/workbench/workbench_types.c
 *
 * PURPOSE:
 *   Implement bounded common Test/Quality workbench value operations.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test/workbench/workbench_types.h"
#include <string.h>
UmiStatus umi_test_workbench_copy_text(char *destination,size_t capacity,const char *source){size_t n;if(destination==NULL||capacity==0U||source==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(source);if(n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(destination,source,n+1U);return UMI_STATUS_OK;}
UmiStatus umi_test_workbench_entry_init(UmiTestWorkbenchEntry *entry,const char *id,const char *label){if(entry==NULL||id==NULL||label==NULL||id[0]=='\0'||label[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(entry,0,sizeof *entry);if(umi_test_workbench_copy_text(entry->id,sizeof entry->id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_test_workbench_copy_text(entry->label,sizeof entry->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;entry->state=UMI_TEST_WORKBENCH_STATE_IDLE;entry->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_test_workbench_entry_set_detail(UmiTestWorkbenchEntry *entry,const char *detail){UmiStatus s;if(entry==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_test_workbench_copy_text(entry->detail,sizeof entry->detail,detail!=NULL?detail:"");if(s==UMI_STATUS_OK)entry->revision++;return s;}
UmiStatus umi_test_workbench_entry_set_state(UmiTestWorkbenchEntry *entry,UmiTestWorkbenchState state){if(entry==NULL||state<UMI_TEST_WORKBENCH_STATE_IDLE||state>UMI_TEST_WORKBENCH_STATE_CANCELLED)return UMI_STATUS_INVALID_ARGUMENT;entry->state=state;entry->revision++;return UMI_STATUS_OK;}
int umi_test_workbench_entry_valid(const UmiTestWorkbenchEntry *entry){return entry!=NULL&&entry->id[0]!='\0'&&entry->label[0]!='\0'&&entry->state>=UMI_TEST_WORKBENCH_STATE_IDLE&&entry->state<=UMI_TEST_WORKBENCH_STATE_CANCELLED;}
