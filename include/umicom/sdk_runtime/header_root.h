/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/header_root.h
 *
 * PURPOSE:
 *   Describe public SDK header roots and aggregate include evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_HEADER_ROOT
#define UMICOM_SDK_RUNTIME_HEADER_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeHeaderRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t header_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeHeaderRoot;
void umi_sdk_runtime_header_root_init(UmiSdkRuntimeHeaderRoot *value,const char *id);
UmiStatus umi_sdk_runtime_header_root_validate(const UmiSdkRuntimeHeaderRoot *value);
UmiStatus umi_sdk_runtime_header_root_set_path(UmiSdkRuntimeHeaderRoot *value,const char *path);
UmiStatus umi_sdk_runtime_header_root_set_detail(UmiSdkRuntimeHeaderRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_header_root_set_header_count(UmiSdkRuntimeHeaderRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_header_root_set_generation(UmiSdkRuntimeHeaderRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_header_root_set_state(UmiSdkRuntimeHeaderRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_header_root_same_identity(const UmiSdkRuntimeHeaderRoot *left,const UmiSdkRuntimeHeaderRoot *right);
#ifdef __cplusplus
}
#endif
#endif
