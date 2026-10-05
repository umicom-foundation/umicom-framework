/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_session.c
 *
 * PURPOSE:
 *   Track binding activation, revision and propagation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_session.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the binding session contract to deterministic zero/default state. */
void umi_ui_reactive_binding_session_init(UmiUiReactiveBindingSession *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_session_valid(const UmiUiReactiveBindingSession *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->session_id, '\0', sizeof(item->session_id)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveBindingSessionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4072236e3d89f93e);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveBindingSession *)0)->session_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveBindingSessionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveBindingSession *)0)->session_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiReactiveBindingSessionArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveBindingSession *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->propagations);
}
static void UmiUiReactiveBindingSessionArchiveRead(UmiArchiveReader *reader, UmiUiReactiveBindingSession *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->propagations = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiUiReactiveBindingSessionArchiveValidate(const UmiUiReactiveBindingSession *value)
{
    return umi_ui_reactive_binding_session_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_binding_session_archive_encode, umi_ui_reactive_binding_session_archive_decode,
    UmiUiReactiveBindingSession, UmiUiReactiveBindingSessionArchiveSchema, UmiUiReactiveBindingSessionArchiveBound, UmiUiReactiveBindingSessionArchiveWrite, UmiUiReactiveBindingSessionArchiveRead, UmiUiReactiveBindingSessionArchiveValidate)
