/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/controller.h
 *
 * PURPOSE:
 *   Implement the Test Runtime Slave Controller lifecycle and command boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CONTROLLER
#define UMICOM_TEST_RUNTIME_CONTROLLER

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeController {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t state;
    uint64_t command_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeController;

void umi_test_runtime_controller_init(UmiTestRuntimeController *value, const char *id);
UmiStatus umi_test_runtime_controller_validate(const UmiTestRuntimeController *value);
UmiStatus umi_test_runtime_controller_set_name(UmiTestRuntimeController *value, const char *name);
UmiStatus umi_test_runtime_controller_set_detail(UmiTestRuntimeController *value, const char *detail);
UmiStatus umi_test_runtime_controller_set_state(UmiTestRuntimeController *value, uint64_t number);
UmiStatus umi_test_runtime_controller_set_command_count(UmiTestRuntimeController *value, uint64_t number);
UmiStatus umi_test_runtime_controller_touch(UmiTestRuntimeController *value, uint64_t updated_at_ms);
bool umi_test_runtime_controller_same_identity(const UmiTestRuntimeController *left, const UmiTestRuntimeController *right);

#ifdef __cplusplus
}
#endif
#endif
