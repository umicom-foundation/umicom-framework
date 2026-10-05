/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/tree_spec.c
 *
 * PURPOSE:
 *   Define virtualised hierarchical tree depth, selection and checkbox semantics.
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

#include "umicom/ui/design/tree_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design tree spec satisfies its contract before another service relies on it. */
int umi_design_tree_spec_valid(const UmiDesignTreeSpec *spec) { return spec!=NULL && (spec->maximum_depth>0U && spec->maximum_depth<=64U) ? 1 : 0; }
/*
 * Initialise design tree spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_tree_spec_init(UmiDesignTreeSpec *spec, uint16_t maximum_depth, int virtualised, int multi_select, int checkboxes)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->maximum_depth=maximum_depth;spec->virtualised=virtualised?1:0;spec->multi_select=multi_select?1:0;spec->checkboxes=checkboxes?1:0;
    return umi_design_tree_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignTreeSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbaa07b960a91a942);

    return schema;
}
static size_t UmiDesignTreeSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignTreeSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignTreeSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_depth);
    UmiArchiveWriteSigned(writer, (int64_t)value->virtualised);
    UmiArchiveWriteSigned(writer, (int64_t)value->multi_select);
    UmiArchiveWriteSigned(writer, (int64_t)value->checkboxes);
}
static void UmiDesignTreeSpecArchiveRead(UmiArchiveReader *reader, UmiDesignTreeSpec *value)
{
    value->maximum_depth = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->virtualised = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->multi_select = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->checkboxes = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignTreeSpecArchiveValidate(const UmiDesignTreeSpec *value)
{
    return umi_design_tree_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_tree_spec_archive_encode, umi_design_tree_spec_archive_decode,
    UmiDesignTreeSpec, UmiDesignTreeSpecArchiveSchema, UmiDesignTreeSpecArchiveBound, UmiDesignTreeSpecArchiveWrite, UmiDesignTreeSpecArchiveRead, UmiDesignTreeSpecArchiveValidate)
