/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/ui_state_service.c
 *
 * PURPOSE:
 *   Implement aggregate readiness of binding, validation and state-graph services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/ui_state_service.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the ui state service contract to deterministic zero/default state. */
void umi_ui_reactive_ui_state_service_init(UmiUiReactiveUiStateService *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_ui_state_service_valid(const UmiUiReactiveUiStateService *item) {
    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveUiStateServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd215cf2a3d29b1b5);

    return schema;
}
static size_t UmiUiReactiveUiStateServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveUiStateServiceArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveUiStateService *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bindings_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->validation_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->graph_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scheduler_ready);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiUiReactiveUiStateServiceArchiveRead(UmiArchiveReader *reader, UmiUiReactiveUiStateService *value)
{
    value->bindings_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->validation_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->graph_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->scheduler_ready = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiReactiveUiStateServiceArchiveValidate(const UmiUiReactiveUiStateService *value)
{
    return umi_ui_reactive_ui_state_service_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_ui_state_service_archive_encode, umi_ui_reactive_ui_state_service_archive_decode,
    UmiUiReactiveUiStateService, UmiUiReactiveUiStateServiceArchiveSchema, UmiUiReactiveUiStateServiceArchiveBound, UmiUiReactiveUiStateServiceArchiveWrite, UmiUiReactiveUiStateServiceArchiveRead, UmiUiReactiveUiStateServiceArchiveValidate)
