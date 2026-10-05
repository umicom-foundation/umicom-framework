/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/text_input_spec.c
 *
 * PURPOSE:
 *   Define text-entry mode, placeholder and validation semantics for reusable inputs.
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

#include "umicom/ui/design/text_input_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design text input spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_text_input_spec_valid(const UmiDesignTextInputSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->placeholder, '\0', sizeof(spec->placeholder)) == NULL) return 0;
 return spec!=NULL && (spec->max_length>0U && !(spec->password && spec->multiline)) ? 1 : 0; }
/*
 * Initialise design text input spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_text_input_spec_init(UmiDesignTextInputSpec *spec, const char *placeholder, uint32_t max_length, int password, int search, int multiline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->max_length=max_length;spec->password=password?1:0;spec->search=search?1:0;spec->multiline=multiline?1:0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(placeholder!=NULL){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_design_copy_text(spec->placeholder,sizeof spec->placeholder,placeholder)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;}
    return umi_design_text_input_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignTextInputSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe2b2723451d77f02);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignTextInputSpec *)0)->placeholder)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignTextInputSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignTextInputSpec *)0)->placeholder) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignTextInputSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignTextInputSpec *value)
{
    UmiArchiveWriteText(writer, value->placeholder, sizeof(value->placeholder));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_length);
    UmiArchiveWriteSigned(writer, (int64_t)value->password);
    UmiArchiveWriteSigned(writer, (int64_t)value->search);
    UmiArchiveWriteSigned(writer, (int64_t)value->multiline);
}
static void UmiDesignTextInputSpecArchiveRead(UmiArchiveReader *reader, UmiDesignTextInputSpec *value)
{
    UmiArchiveReadText(reader, value->placeholder, sizeof(value->placeholder));
    value->max_length = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->password = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->search = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->multiline = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignTextInputSpecArchiveValidate(const UmiDesignTextInputSpec *value)
{
    return umi_design_text_input_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_text_input_spec_archive_encode, umi_design_text_input_spec_archive_decode,
    UmiDesignTextInputSpec, UmiDesignTextInputSpecArchiveSchema, UmiDesignTextInputSpecArchiveBound, UmiDesignTextInputSpecArchiveWrite, UmiDesignTextInputSpecArchiveRead, UmiDesignTextInputSpecArchiveValidate)
