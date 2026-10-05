/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/animation_gate.c
 *
 * PURPOSE:
 *   Decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/animation_gate.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_animation_gate_init(UmiAppearanceAnimationGate *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->animation_id,sizeof item->animation_id,"progress.pulse");
    item->essential=true;
    item->reduced_motion=false;
    item->allowed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_animation_gate_is_valid(const UmiAppearanceAnimationGate *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->animation_id, '\0', sizeof(item->animation_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->animation_id) && (!item->reduced_motion || item->essential || !item->allowed));
}
/*
 * Provide the appearance animation gate resolve operation used by this module and its
 * client applications.
 */
void umi_appearance_animation_gate_resolve(UmiAppearanceAnimationGate *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item!=NULL)item->allowed=(!item->reduced_motion)||item->essential;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAnimationGateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x63d9ac1ef5f61f75);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAnimationGate *)0)->animation_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAnimationGateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAnimationGate *)0)->animation_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAnimationGateArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAnimationGate *value)
{
    UmiArchiveWriteText(writer, value->animation_id, sizeof(value->animation_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->essential);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed);
}
static void UmiAppearanceAnimationGateArchiveRead(UmiArchiveReader *reader, UmiAppearanceAnimationGate *value)
{
    UmiArchiveReadText(reader, value->animation_id, sizeof(value->animation_id));
    value->essential = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceAnimationGateArchiveValidate(const UmiAppearanceAnimationGate *value)
{
    return umi_appearance_animation_gate_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_animation_gate_archive_encode, umi_appearance_animation_gate_archive_decode,
    UmiAppearanceAnimationGate, UmiAppearanceAnimationGateArchiveSchema, UmiAppearanceAnimationGateArchiveBound, UmiAppearanceAnimationGateArchiveWrite, UmiAppearanceAnimationGateArchiveRead, UmiAppearanceAnimationGateArchiveValidate)
