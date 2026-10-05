/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/list_spec.c
 *
 * PURPOSE:
 *   Define virtualised list selection, estimated size and row-density semantics.
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

#include "umicom/ui/design/list_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design list spec satisfies its contract before another service relies on it. */
int umi_design_list_spec_valid(const UmiDesignListSpec *spec) { return spec!=NULL && (spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && (spec->estimated_items==0U || spec->virtualised || spec->estimated_items<=1000U)) ? 1 : 0; }
/*
 * Initialise design list spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_list_spec_init(UmiDesignListSpec *spec, size_t estimated_items, UmiDesignDensity density, int virtualised, int multi_select)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->estimated_items=estimated_items;spec->density=density;spec->virtualised=virtualised?1:0;spec->multi_select=multi_select?1:0;
    return umi_design_list_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignListSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf044d350603d9f58);

    return schema;
}
static size_t UmiDesignListSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignListSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignListSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->estimated_items);
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteSigned(writer, (int64_t)value->virtualised);
    UmiArchiveWriteSigned(writer, (int64_t)value->multi_select);
}
static void UmiDesignListSpecArchiveRead(UmiArchiveReader *reader, UmiDesignListSpec *value)
{
    value->estimated_items = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->virtualised = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->multi_select = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignListSpecArchiveValidate(const UmiDesignListSpec *value)
{
    return umi_design_list_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_list_spec_archive_encode, umi_design_list_spec_archive_decode,
    UmiDesignListSpec, UmiDesignListSpecArchiveSchema, UmiDesignListSpecArchiveBound, UmiDesignListSpecArchiveWrite, UmiDesignListSpecArchiveRead, UmiDesignListSpecArchiveValidate)
