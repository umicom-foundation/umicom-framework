/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/table_spec.c
 *
 * PURPOSE:
 *   Define enterprise table column, virtualisation, sorting, filtering and frozen-column semantics.
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

#include "umicom/ui/design/table_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design table spec satisfies its contract before another service relies on it. */
int umi_design_table_spec_valid(const UmiDesignTableSpec *spec) { return spec!=NULL && (spec->columns>0U && spec->frozen_columns<=spec->columns && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH) ? 1 : 0; }
/*
 * Initialise design table spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_table_spec_init(UmiDesignTableSpec *spec, uint16_t columns, uint16_t frozen_columns, UmiDesignDensity density, int virtualised, int sortable, int filterable)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->columns=columns;spec->frozen_columns=frozen_columns;spec->density=density;spec->virtualised=virtualised?1:0;spec->sortable=sortable?1:0;spec->filterable=filterable?1:0;
    return umi_design_table_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignTableSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf1b1b23a4dea9f93);

    return schema;
}
static size_t UmiDesignTableSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignTableSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignTableSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->columns);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frozen_columns);
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteSigned(writer, (int64_t)value->virtualised);
    UmiArchiveWriteSigned(writer, (int64_t)value->sortable);
    UmiArchiveWriteSigned(writer, (int64_t)value->filterable);
}
static void UmiDesignTableSpecArchiveRead(UmiArchiveReader *reader, UmiDesignTableSpec *value)
{
    value->columns = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->frozen_columns = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->virtualised = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sortable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->filterable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignTableSpecArchiveValidate(const UmiDesignTableSpec *value)
{
    return umi_design_table_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_table_spec_archive_encode, umi_design_table_spec_archive_decode,
    UmiDesignTableSpec, UmiDesignTableSpecArchiveSchema, UmiDesignTableSpecArchiveBound, UmiDesignTableSpecArchiveWrite, UmiDesignTableSpecArchiveRead, UmiDesignTableSpecArchiveValidate)
