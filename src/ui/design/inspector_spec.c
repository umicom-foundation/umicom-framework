/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/inspector_spec.c
 *
 * PURPOSE:
 *   Define searchable property-inspector grouping and advanced-property presentation.
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

#include "umicom/ui/design/inspector_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design inspector spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_inspector_spec_valid(const UmiDesignInspectorSpec *spec) { return spec!=NULL && (spec->category_count>0U && spec->category_count<=64U && spec->estimated_properties<=100000U) ? 1 : 0; }
/*
 * Initialise design inspector spec from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_design_inspector_spec_init(UmiDesignInspectorSpec *spec, uint16_t category_count, size_t estimated_properties, int searchable, int advanced_toggle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->category_count=category_count;spec->estimated_properties=estimated_properties;spec->searchable=searchable?1:0;spec->advanced_toggle=advanced_toggle?1:0;
    return umi_design_inspector_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignInspectorSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x42b0f12f5c893bd9);

    return schema;
}
static size_t UmiDesignInspectorSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignInspectorSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignInspectorSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->category_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->estimated_properties);
    UmiArchiveWriteSigned(writer, (int64_t)value->searchable);
    UmiArchiveWriteSigned(writer, (int64_t)value->advanced_toggle);
}
static void UmiDesignInspectorSpecArchiveRead(UmiArchiveReader *reader, UmiDesignInspectorSpec *value)
{
    value->category_count = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->estimated_properties = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->searchable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->advanced_toggle = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignInspectorSpecArchiveValidate(const UmiDesignInspectorSpec *value)
{
    return umi_design_inspector_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_inspector_spec_archive_encode, umi_design_inspector_spec_archive_decode,
    UmiDesignInspectorSpec, UmiDesignInspectorSpecArchiveSchema, UmiDesignInspectorSpecArchiveBound, UmiDesignInspectorSpecArchiveWrite, UmiDesignInspectorSpecArchiveRead, UmiDesignInspectorSpecArchiveValidate)
