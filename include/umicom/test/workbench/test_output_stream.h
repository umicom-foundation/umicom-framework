/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_stream.h
 *
 * PURPOSE:
 *   Model test output stream state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_STREAM_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_STREAM_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestOutputStream {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputStream;
UmiStatus umi_test_output_stream_init(UmiTestOutputStream *model,const char *id,const char *label);
UmiStatus umi_test_output_stream_set_active(UmiTestOutputStream *model,bool active);
UmiStatus umi_test_output_stream_set_count(UmiTestOutputStream *model,uint32_t item_count);
UmiStatus umi_test_output_stream_set_state(UmiTestOutputStream *model,UmiTestWorkbenchState state);
int umi_test_output_stream_valid(const UmiTestOutputStream *model);
#ifdef __cplusplus
}
#endif
#endif
