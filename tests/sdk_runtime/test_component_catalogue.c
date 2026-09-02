/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_component_catalogue.c
 *
 * PURPOSE:
 *   Verify the component catalogue contract and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/component_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeComponentCatalogue value; UmiSdkRuntimeComponentCatalogue same; uint64_t revision;
    umi_sdk_runtime_component_catalogue_init(&value, "sdk-runtime.component_catalogue");
    assert(umi_sdk_runtime_component_catalogue_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_component_catalogue_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_catalogue_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_catalogue_set_component_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_catalogue_set_generation(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_catalogue_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.component_count == 3U && value.generation == 5U);
    umi_sdk_runtime_component_catalogue_init(&same, "sdk-runtime.component_catalogue");
    assert(umi_sdk_runtime_component_catalogue_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
