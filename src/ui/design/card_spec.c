/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/card_spec.c
 *
 * PURPOSE:
 *   Define reusable card elevation, interaction and semantic intent.
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

#include "umicom/ui/design/card_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design card spec satisfies its contract before another service relies on it. */
int umi_design_card_spec_valid(const UmiDesignCardSpec *spec) { return spec!=NULL && (spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT && spec->elevation_level<=5U) ? 1 : 0; }
/*
 * Initialise design card spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_card_spec_init(UmiDesignCardSpec *spec, UmiDesignSemanticRole role, uint8_t elevation_level, int interactive, int selected)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->role=role;spec->elevation_level=elevation_level;spec->interactive=interactive?1:0;spec->selected=selected?1:0;
    return umi_design_card_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignCardSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x460c200193c48386);

    return schema;
}
static size_t UmiDesignCardSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignCardSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignCardSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->role);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->elevation_level);
    UmiArchiveWriteSigned(writer, (int64_t)value->interactive);
    UmiArchiveWriteSigned(writer, (int64_t)value->selected);
}
static void UmiDesignCardSpecArchiveRead(UmiArchiveReader *reader, UmiDesignCardSpec *value)
{
    value->role = (UmiDesignSemanticRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->elevation_level = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->interactive = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->selected = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignCardSpecArchiveValidate(const UmiDesignCardSpec *value)
{
    return umi_design_card_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_card_spec_archive_encode, umi_design_card_spec_archive_decode,
    UmiDesignCardSpec, UmiDesignCardSpecArchiveSchema, UmiDesignCardSpecArchiveBound, UmiDesignCardSpecArchiveWrite, UmiDesignCardSpecArchiveRead, UmiDesignCardSpecArchiveValidate)
