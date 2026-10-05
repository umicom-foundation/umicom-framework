/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/mnemonic_policy.c
 *
 * PURPOSE:
 *   Govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/mnemonic_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_mnemonic_policy_init(UmiAppearanceMnemonicPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"mnemonic.default");
    item->show_on_keyboard_intent=true;
    item->unique_within_scope=true;
    item->localised=true;
    item->allow_auto_assignment=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_mnemonic_policy_is_valid(const UmiAppearanceMnemonicPolicy *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->policy_id, '\0', sizeof(item->policy_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->unique_within_scope);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceMnemonicPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x77642f2c8e5f0d80);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceMnemonicPolicy *)0)->policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceMnemonicPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceMnemonicPolicy *)0)->policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceMnemonicPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceMnemonicPolicy *value)
{
    UmiArchiveWriteText(writer, value->policy_id, sizeof(value->policy_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->show_on_keyboard_intent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->unique_within_scope);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->localised);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_auto_assignment);
}
static void UmiAppearanceMnemonicPolicyArchiveRead(UmiArchiveReader *reader, UmiAppearanceMnemonicPolicy *value)
{
    UmiArchiveReadText(reader, value->policy_id, sizeof(value->policy_id));
    value->show_on_keyboard_intent = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->unique_within_scope = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->localised = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_auto_assignment = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceMnemonicPolicyArchiveValidate(const UmiAppearanceMnemonicPolicy *value)
{
    return umi_appearance_mnemonic_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_mnemonic_policy_archive_encode, umi_appearance_mnemonic_policy_archive_decode,
    UmiAppearanceMnemonicPolicy, UmiAppearanceMnemonicPolicyArchiveSchema, UmiAppearanceMnemonicPolicyArchiveBound, UmiAppearanceMnemonicPolicyArchiveWrite, UmiAppearanceMnemonicPolicyArchiveRead, UmiAppearanceMnemonicPolicyArchiveValidate)
