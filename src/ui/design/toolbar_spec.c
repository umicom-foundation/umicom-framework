/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/toolbar_spec.c
 *
 * PURPOSE:
 *   Define toolbar orientation, density and overflow behaviour for reusable command surfaces.
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

#include "umicom/ui/design/toolbar_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design toolbar spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_toolbar_spec_valid(const UmiDesignToolbarSpec *spec) { return spec!=NULL && ((spec->orientation==UMI_UI_HORIZONTAL || spec->orientation==UMI_UI_VERTICAL) && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && spec->preferred_items>0U) ? 1 : 0; }
/*
 * Initialise design toolbar spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_toolbar_spec_init(UmiDesignToolbarSpec *spec, UmiUiOrientation orientation, UmiDesignDensity density, uint16_t preferred_items, int overflow_menu)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->orientation=orientation;spec->density=density;spec->preferred_items=preferred_items;spec->overflow_menu=overflow_menu?1:0;
    return umi_design_toolbar_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignToolbarSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb28c78a1b273d75b);

    return schema;
}
static size_t UmiDesignToolbarSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignToolbarSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignToolbarSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->preferred_items);
    UmiArchiveWriteSigned(writer, (int64_t)value->overflow_menu);
}
static void UmiDesignToolbarSpecArchiveRead(UmiArchiveReader *reader, UmiDesignToolbarSpec *value)
{
    value->orientation = (UmiUiOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->preferred_items = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->overflow_menu = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignToolbarSpecArchiveValidate(const UmiDesignToolbarSpec *value)
{
    return umi_design_toolbar_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_toolbar_spec_archive_encode, umi_design_toolbar_spec_archive_decode,
    UmiDesignToolbarSpec, UmiDesignToolbarSpecArchiveSchema, UmiDesignToolbarSpecArchiveBound, UmiDesignToolbarSpecArchiveWrite, UmiDesignToolbarSpecArchiveRead, UmiDesignToolbarSpecArchiveValidate)
