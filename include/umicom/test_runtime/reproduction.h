/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/reproduction.h
 *
 * PURPOSE:
 *   Describe a deterministic command and environment for reproducing one failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_REPRODUCTION
#define UMICOM_TEST_RUNTIME_REPRODUCTION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeReproduction {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t step_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeReproduction;
void umi_test_runtime_reproduction_init(UmiTestRuntimeReproduction *value,const char *id);
UmiStatus umi_test_runtime_reproduction_validate(const UmiTestRuntimeReproduction *value);
UmiStatus umi_test_runtime_reproduction_set_category(UmiTestRuntimeReproduction *value,const char *category);
UmiStatus umi_test_runtime_reproduction_set_detail(UmiTestRuntimeReproduction *value,const char *detail);
UmiStatus umi_test_runtime_reproduction_set_step_count(UmiTestRuntimeReproduction *value,uint64_t number);
UmiStatus umi_test_runtime_reproduction_set_generation(UmiTestRuntimeReproduction *value,uint64_t number);
UmiStatus umi_test_runtime_reproduction_set_active(UmiTestRuntimeReproduction *value,bool active);
bool umi_test_runtime_reproduction_same_identity(const UmiTestRuntimeReproduction *left,const UmiTestRuntimeReproduction *right);
#ifdef __cplusplus
}
#endif
#endif
