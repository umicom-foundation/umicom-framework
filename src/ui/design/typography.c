/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/typography.c
 *
 * PURPOSE:
 *   Define validated toolkit-neutral typography specifications for semantic text roles.
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

#include "umicom/ui/design/typography.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise design typography from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_typography_init(UmiDesignTypography *spec,const char *family,double size,uint16_t weight,double line_height)
{ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(spec==NULL||family==NULL||!umi_design_number_valid(size)||!umi_design_number_valid(line_height)||size<=0.0||line_height<1.0||weight<100U||weight>1000U)return UMI_STATUS_INVALID_ARGUMENT; memset(spec,0,sizeof *spec); s=umi_design_copy_text(spec->family,sizeof spec->family,family); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; spec->size=size;spec->weight=weight;spec->line_height=line_height;return UMI_STATUS_OK; }
/* Check that design typography satisfies its contract before another service relies on it. */
int umi_design_typography_valid(const UmiDesignTypography *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->family, '\0', sizeof(spec->family)) == NULL) return 0;
 return spec!=NULL && spec->family[0]!='\0' && umi_design_number_valid(spec->size) && spec->size>0.0 && spec->weight>=100U && spec->weight<=1000U && umi_design_number_valid(spec->line_height) && spec->line_height>=1.0; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignTypographyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc9b64323c177f708);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignTypography *)0)->family)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignTypographyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignTypography *)0)->family) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignTypographyArchiveWrite(UmiArchiveWriter *writer, const UmiDesignTypography *value)
{
    UmiArchiveWriteText(writer, value->family, sizeof(value->family));
    UmiArchiveWriteDouble(writer, value->size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
    UmiArchiveWriteDouble(writer, value->line_height);
    UmiArchiveWriteDouble(writer, value->letter_spacing);
}
static void UmiDesignTypographyArchiveRead(UmiArchiveReader *reader, UmiDesignTypography *value)
{
    UmiArchiveReadText(reader, value->family, sizeof(value->family));
    value->size = UmiArchiveReadDouble(reader);
    value->weight = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->line_height = UmiArchiveReadDouble(reader);
    value->letter_spacing = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiDesignTypographyArchiveValidate(const UmiDesignTypography *value)
{
    return umi_design_typography_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_typography_archive_encode, umi_design_typography_archive_decode,
    UmiDesignTypography, UmiDesignTypographyArchiveSchema, UmiDesignTypographyArchiveBound, UmiDesignTypographyArchiveWrite, UmiDesignTypographyArchiveRead, UmiDesignTypographyArchiveValidate)
