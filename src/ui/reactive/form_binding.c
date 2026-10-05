/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/form_binding.c
 *
 * PURPOSE:
 *   Implement form-level model binding and commit policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/form_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise the form binding contract to deterministic zero/default state. */
void umi_ui_reactive_form_binding_init(UmiUiReactiveFormBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_form_binding_valid(const UmiUiReactiveFormBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->form_id, '\0', sizeof(item->form_id)) == NULL) return 0;
    if (memchr(item->model_prefix, '\0', sizeof(item->model_prefix)) == NULL) return 0;

    return item != NULL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiReactiveFormBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfcbd0fc5b0bdefe9);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveFormBinding *)0)->form_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiReactiveFormBinding *)0)->model_prefix)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiReactiveFormBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiReactiveFormBinding *)0)->form_id) - 1U +
        8U + sizeof(((UmiUiReactiveFormBinding *)0)->model_prefix) - 1U +
        8U +
        8U;
}
static void UmiUiReactiveFormBindingArchiveWrite(UmiArchiveWriter *writer, const UmiUiReactiveFormBinding *value)
{
    UmiArchiveWriteText(writer, value->form_id, sizeof(value->form_id));
    UmiArchiveWriteText(writer, value->model_prefix, sizeof(value->model_prefix));
    UmiArchiveWriteSigned(writer, (int64_t)value->trigger);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->validate_before_commit);
}
static void UmiUiReactiveFormBindingArchiveRead(UmiArchiveReader *reader, UmiUiReactiveFormBinding *value)
{
    UmiArchiveReadText(reader, value->form_id, sizeof(value->form_id));
    UmiArchiveReadText(reader, value->model_prefix, sizeof(value->model_prefix));
    value->trigger = (UmiUiReactiveUpdateTrigger)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->validate_before_commit = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiUiReactiveFormBindingArchiveValidate(const UmiUiReactiveFormBinding *value)
{
    return umi_ui_reactive_form_binding_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_reactive_form_binding_archive_encode, umi_ui_reactive_form_binding_archive_decode,
    UmiUiReactiveFormBinding, UmiUiReactiveFormBindingArchiveSchema, UmiUiReactiveFormBindingArchiveBound, UmiUiReactiveFormBindingArchiveWrite, UmiUiReactiveFormBindingArchiveRead, UmiUiReactiveFormBindingArchiveValidate)
