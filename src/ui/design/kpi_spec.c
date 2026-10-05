/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/kpi_spec.c
 *
 * PURPOSE:
 *   Define dashboard KPI value, trend and semantic status presentation.
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

#include "umicom/ui/design/kpi_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design kpi spec satisfies its contract before another service relies on it. */
int umi_design_kpi_spec_valid(const UmiDesignKpiSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->label, '\0', sizeof(spec->label)) == NULL) return 0;
 return spec!=NULL && (spec->label[0]!='\0' && umi_design_number_valid(spec->value) && umi_design_number_valid(spec->change) && spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
/*
 * Initialise design kpi spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_kpi_spec_init(UmiDesignKpiSpec *spec, const char *label, double value, double change, UmiDesignSemanticRole role, int percentage)
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
        if (label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->label, sizeof spec->label, label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->value = value;
    spec->change = change;
    spec->role = role;
    spec->percentage = percentage ? 1 : 0;
    return umi_design_kpi_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignKpiSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x57bb132c5cbe77e8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignKpiSpec *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignKpiSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignKpiSpec *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignKpiSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignKpiSpec *value)
{
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->change);
    UmiArchiveWriteSigned(writer, (int64_t)value->role);
    UmiArchiveWriteSigned(writer, (int64_t)value->percentage);
}
static void UmiDesignKpiSpecArchiveRead(UmiArchiveReader *reader, UmiDesignKpiSpec *value)
{
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->value = UmiArchiveReadDouble(reader);
    value->change = UmiArchiveReadDouble(reader);
    value->role = (UmiDesignSemanticRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->percentage = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignKpiSpecArchiveValidate(const UmiDesignKpiSpec *value)
{
    return umi_design_kpi_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_kpi_spec_archive_encode, umi_design_kpi_spec_archive_decode,
    UmiDesignKpiSpec, UmiDesignKpiSpecArchiveSchema, UmiDesignKpiSpecArchiveBound, UmiDesignKpiSpecArchiveWrite, UmiDesignKpiSpecArchiveRead, UmiDesignKpiSpecArchiveValidate)
