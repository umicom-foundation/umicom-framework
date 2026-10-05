/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_scheduler.c
 *
 * PURPOSE:
 *   Track pending propagation work and deterministic dispatch generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_scheduler.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the binding scheduler contract to deterministic zero/default state. */
void umi_ui_reactive_binding_scheduler_init(UmiUiReactiveBindingScheduler *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_scheduler_valid(const UmiUiReactiveBindingScheduler *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBindingSchedulerArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf9f33f3cef660724);

    return schema;
}
static size_t UmiUiReactiveBindingSchedulerArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveBindingSchedulerArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBindingScheduler *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pending);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->suspended);
}
static void UmiUiReactiveBindingSchedulerArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBindingScheduler *value)
{
    value->pending = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->suspended = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveBindingSchedulerArchiveValidate(const UmiUiReactiveBindingScheduler *value)
{
    return umi_ui_reactive_binding_scheduler_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_binding_scheduler_archive_encode, umi_ui_reactive_binding_scheduler_archive_decode,
    UmiUiReactiveBindingScheduler, UmiUiReactiveBindingSchedulerArchiveSchema, UmiUiReactiveBindingSchedulerArchiveBound, UmiUiReactiveBindingSchedulerArchiveWrite, UmiUiReactiveBindingSchedulerArchiveRead, UmiUiReactiveBindingSchedulerArchiveValidate)
