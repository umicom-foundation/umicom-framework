/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/menu_spec.c
 *
 * PURPOSE:
 *   Define scalable menu presentation, search and overflow semantics.
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

#include "umicom/ui/design/menu_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design menu spec satisfies its contract before another service relies on it. */
int umi_design_menu_spec_valid(const UmiDesignMenuSpec *spec) { return spec!=NULL && (spec->max_visible_items>0U) ? 1 : 0; }
/*
 * Initialise design menu spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_menu_spec_init(UmiDesignMenuSpec *spec, uint16_t max_visible_items, int searchable, int icons, int accelerators)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->max_visible_items=max_visible_items;spec->searchable=searchable?1:0;spec->icons=icons?1:0;spec->accelerators=accelerators?1:0;
    return umi_design_menu_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignMenuSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd8165a7ee234b8db);

    return schema;
}
static size_t UmiDesignMenuSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignMenuSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignMenuSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_visible_items);
    UmiArchiveWriteSigned(writer, (int64_t)value->searchable);
    UmiArchiveWriteSigned(writer, (int64_t)value->icons);
    UmiArchiveWriteSigned(writer, (int64_t)value->accelerators);
}
static void UmiDesignMenuSpecArchiveRead(UmiArchiveReader *reader, UmiDesignMenuSpec *value)
{
    value->max_visible_items = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->searchable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->icons = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->accelerators = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignMenuSpecArchiveValidate(const UmiDesignMenuSpec *value)
{
    return umi_design_menu_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_menu_spec_archive_encode, umi_design_menu_spec_archive_decode,
    UmiDesignMenuSpec, UmiDesignMenuSpecArchiveSchema, UmiDesignMenuSpecArchiveBound, UmiDesignMenuSpecArchiveWrite, UmiDesignMenuSpecArchiveRead, UmiDesignMenuSpecArchiveValidate)
