/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/button_spec.c
 *
 * PURPOSE:
 *   Define button intent, size, label and icon semantics independent of frontend toolkit.
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

#include "umicom/ui/design/button_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design button spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_button_spec_valid(const UmiDesignButtonSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->label, '\0', sizeof(spec->label)) == NULL) return 0;
 return spec!=NULL && (spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && (spec->icon_only || spec->label[0]!='\0')) ? 1 : 0; }
/*
 * Initialise design button spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_button_spec_init(UmiDesignButtonSpec *spec, const char *label, UmiDesignSemanticRole role, UmiDesignDensity density, int icon_only, int destructive)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->role=role; spec->density=density; spec->icon_only=icon_only?1:0; spec->destructive=destructive?1:0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(label!=NULL){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_design_copy_text(spec->label,sizeof spec->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; }
    return umi_design_button_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignButtonSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1ff2c5f217fa76a2);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignButtonSpec *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignButtonSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignButtonSpec *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignButtonSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignButtonSpec *value)
{
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->role);
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteSigned(writer, (int64_t)value->icon_only);
    UmiArchiveWriteSigned(writer, (int64_t)value->destructive);
}
static void UmiDesignButtonSpecArchiveRead(UmiArchiveReader *reader, UmiDesignButtonSpec *value)
{
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->role = (UmiDesignSemanticRole)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->icon_only = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->destructive = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignButtonSpecArchiveValidate(const UmiDesignButtonSpec *value)
{
    return umi_design_button_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_button_spec_archive_encode, umi_design_button_spec_archive_decode,
    UmiDesignButtonSpec, UmiDesignButtonSpecArchiveSchema, UmiDesignButtonSpecArchiveBound, UmiDesignButtonSpecArchiveWrite, UmiDesignButtonSpecArchiveRead, UmiDesignButtonSpecArchiveValidate)
