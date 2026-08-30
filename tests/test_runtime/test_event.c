/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_event.c
 *
 * PURPOSE:
 *   Verify the event contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/event.h"

int main(void)
{
    UmiTestRuntimeEvent value;
    UmiTestRuntimeEvent same;
    uint64_t revision;
    umi_test_runtime_event_init(&value, "test-runtime.event");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_event_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_event_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_event_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_event_set_kind(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_event_set_sequence(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_event_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.kind == 7U);
    assert(value.sequence == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_event_init(&same, "test-runtime.event");
    assert(umi_test_runtime_event_same_identity(&value, &same));
    assert(umi_test_runtime_event_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
