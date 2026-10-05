/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/badge_spec.c
 *
 * PURPOSE:
 *   Define compact status and metadata badges with semantic intent.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/badge_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design badge spec satisfies its contract before another service relies on it. */
int umi_design_badge_spec_valid(const UmiDesignBadgeSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->text, '\0', sizeof(spec->text)) == NULL) return 0;
 return spec!=NULL && (spec->text[0]!='\0' && spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
/*
 * Initialise design badge spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_badge_spec_init(UmiDesignBadgeSpec *spec, const char *text, UmiDesignSemanticRole role, int outlined)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->text, sizeof spec->text, text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->role = role;
    spec->outlined = outlined ? 1 : 0;
    return umi_design_badge_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignBadgeSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x360a6ed2aacea2a8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignBadgeSpec *)0)->text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignBadgeSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignBadgeSpec *)0)->text) - 1U +
        8U +
        8U;
}
static void UmiDesignBadgeSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignBadgeSpec *value)
{
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteSigned(writer, (int64_t)value->role);
    UmiArchiveWriteSigned(writer, (int64_t)value->outlined);
}
static void UmiDesignBadgeSpecArchiveRead(UmiArchiveReader *reader, UmiDesignBadgeSpec *value)
{
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    value->role = (UmiDesignSemanticRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->outlined = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignBadgeSpecArchiveValidate(const UmiDesignBadgeSpec *value)
{
    return umi_design_badge_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_badge_spec_archive_encode, umi_design_badge_spec_archive_decode,
    UmiDesignBadgeSpec, UmiDesignBadgeSpecArchiveSchema, UmiDesignBadgeSpecArchiveBound, UmiDesignBadgeSpecArchiveWrite, UmiDesignBadgeSpecArchiveRead, UmiDesignBadgeSpecArchiveValidate)
