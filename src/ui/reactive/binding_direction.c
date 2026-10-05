/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_direction.c
 *
 * PURPOSE:
 *   Implement binding direction and update trigger policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_direction.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the binding direction contract to deterministic zero/default state. */
void umi_ui_reactive_binding_direction_init(UmiUiReactiveBindingDirectionPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_direction_valid(const UmiUiReactiveBindingDirectionPolicy *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBindingDirectionPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1e3369d705a1ab49);

    return schema;
}
static size_t UmiUiReactiveBindingDirectionPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveBindingDirectionPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBindingDirectionPolicy *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->direction);
    UmiArchiveWriteSigned(writer, (int64_t)value->trigger);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->propagate_initial);
}
static void UmiUiReactiveBindingDirectionPolicyArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBindingDirectionPolicy *value)
{
    value->direction = (UmiUiReactiveBindingDirection)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->trigger = (UmiUiReactiveUpdateTrigger)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->propagate_initial = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBindingDirectionPolicyArchiveValidate(const UmiUiReactiveBindingDirectionPolicy *value)
{
    return umi_ui_reactive_binding_direction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_binding_direction_archive_encode, umi_ui_reactive_binding_direction_archive_decode,
    UmiUiReactiveBindingDirectionPolicy, UmiUiReactiveBindingDirectionPolicyArchiveSchema, UmiUiReactiveBindingDirectionPolicyArchiveBound, UmiUiReactiveBindingDirectionPolicyArchiveWrite, UmiUiReactiveBindingDirectionPolicyArchiveRead, UmiUiReactiveBindingDirectionPolicyArchiveValidate)
