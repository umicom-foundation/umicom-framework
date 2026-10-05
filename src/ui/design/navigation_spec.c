/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/navigation_spec.c
 *
 * PURPOSE:
 *   Define rail, sidebar and bottom-navigation semantics across desktop, web and mobile compositions.
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

#include "umicom/ui/design/navigation_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design navigation spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_navigation_spec_valid(const UmiDesignNavigationSpec *spec) { return spec!=NULL && (spec->placement>=UMI_UI_PLACEMENT_LEFT && spec->placement<=UMI_UI_PLACEMENT_BOTTOM && spec->item_count>0U) ? 1 : 0; }
/*
 * Initialise design navigation spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_navigation_spec_init(UmiDesignNavigationSpec *spec, UmiUiPlacement placement, uint16_t item_count, int collapsible, int show_labels)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->placement=placement;spec->item_count=item_count;spec->collapsible=collapsible?1:0;spec->show_labels=show_labels?1:0;
    return umi_design_navigation_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignNavigationSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc105a2fc73e47080);

    return schema;
}
static size_t UmiDesignNavigationSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignNavigationSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignNavigationSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->placement);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->collapsible);
    UmiArchiveWriteSigned(writer, (int64_t)value->show_labels);
}
static void UmiDesignNavigationSpecArchiveRead(UmiArchiveReader *reader, UmiDesignNavigationSpec *value)
{
    value->placement = (UmiUiPlacement)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->item_count = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->collapsible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->show_labels = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignNavigationSpecArchiveValidate(const UmiDesignNavigationSpec *value)
{
    return umi_design_navigation_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_navigation_spec_archive_encode, umi_design_navigation_spec_archive_decode,
    UmiDesignNavigationSpec, UmiDesignNavigationSpecArchiveSchema, UmiDesignNavigationSpecArchiveBound, UmiDesignNavigationSpecArchiveWrite, UmiDesignNavigationSpecArchiveRead, UmiDesignNavigationSpecArchiveValidate)
