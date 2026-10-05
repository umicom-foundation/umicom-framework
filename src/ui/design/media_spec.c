/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/media_spec.c
 *
 * PURPOSE:
 *   Define image, audio and video media presentation and transport-control semantics.
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

#include "umicom/ui/design/media_spec.h"
#include "../../base/value_archive_internal.h"

/* Check that design media spec satisfies its contract before another service relies on it. */
int umi_design_media_spec_valid(const UmiDesignMediaSpec *spec){return spec!=NULL&&spec->kind>=UMI_DESIGN_MEDIA_IMAGE&&spec->kind<=UMI_DESIGN_MEDIA_VIDEO&&!(spec->kind==UMI_DESIGN_MEDIA_IMAGE&&spec->autoplay)?1:0;}
/*
 * Initialise design media spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_media_spec_init(UmiDesignMediaSpec *spec,UmiDesignMediaKind kind,int controls,int autoplay,int loop,int preserve_aspect){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(spec==NULL)return UMI_STATUS_INVALID_ARGUMENT;spec->kind=kind;spec->controls=controls?1:0;spec->autoplay=autoplay?1:0;spec->loop=loop?1:0;spec->preserve_aspect=preserve_aspect?1:0;return umi_design_media_spec_valid(spec)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignMediaSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdb4ab556f54896d7);

    return schema;
}
static size_t UmiDesignMediaSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignMediaSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignMediaSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->controls);
    UmiArchiveWriteSigned(writer, (int64_t)value->autoplay);
    UmiArchiveWriteSigned(writer, (int64_t)value->loop);
    UmiArchiveWriteSigned(writer, (int64_t)value->preserve_aspect);
}
static void UmiDesignMediaSpecArchiveRead(UmiArchiveReader *reader, UmiDesignMediaSpec *value)
{
    value->kind = (UmiDesignMediaKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->controls = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->autoplay = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->loop = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->preserve_aspect = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignMediaSpecArchiveValidate(const UmiDesignMediaSpec *value)
{
    return umi_design_media_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_media_spec_archive_encode, umi_design_media_spec_archive_decode,
    UmiDesignMediaSpec, UmiDesignMediaSpecArchiveSchema, UmiDesignMediaSpecArchiveBound, UmiDesignMediaSpecArchiveWrite, UmiDesignMediaSpecArchiveRead, UmiDesignMediaSpecArchiveValidate)
