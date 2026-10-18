/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/package.c
 *
 * PURPOSE:
 *   Implement package specifications and results without binding products to one archive or installer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The same Studio release can be packaged as a directory, ZIP, Windows installer or another format through one contract.
 */

#include "../base/conditional_archive_internal.h"
#include "umicom/delivery/package.h"
#include "../base/value_construction_internal.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise package spec from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_package_spec_init(UmiPackageSpec *spec,
                                const char *package_id,
                                UmiPackageFormat format,
                                const char *staging_root,
                                const char *output_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || package_id == NULL || staging_root == NULL ||
        output_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(spec, 0, sizeof(*spec));
    status = umi_delivery_copy_text(spec->package_id, sizeof(spec->package_id), package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->staging_root, sizeof(spec->staging_root), staging_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->output_path, sizeof(spec->output_path), output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    spec->format = format;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_package_spec_init_build_value(UmiPackageSpec *spec,
                                const char *package_id,
                                UmiPackageFormat format,
                                const char *staging_root,
                                const char *output_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL || package_id == NULL || staging_root == NULL ||
        output_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(spec, 0, sizeof(*spec));
    status = umi_delivery_copy_text(spec->package_id, sizeof(spec->package_id), package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->staging_root, sizeof(spec->staging_root), staging_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(spec->output_path, sizeof(spec->output_path), output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    spec->format = format;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_package_spec_init, UmiPackageSpec, spec,
    (UmiPackageSpec *spec, const char *package_id, UmiPackageFormat format, const char *staging_root, const char *output_path),
    umi_package_spec_init_build_value(candidate, package_id, format, staging_root, output_path))


/* Check that package spec satisfies its contract before another service relies on it. */
UmiStatus umi_package_spec_validate(const UmiPackageSpec *spec)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->package_id, '\0', sizeof(spec->package_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->staging_root, '\0', sizeof(spec->staging_root)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(spec->output_path, '\0', sizeof(spec->output_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (spec->package_id[0] == '\0' || spec->staging_root[0] == '\0' ||
        spec->output_path[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Initialise package result from caller-provided values so later operations receive a
 * known state.
 */
void umi_package_result_init(UmiPackageResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result != NULL) (void)memset(result, 0, sizeof(*result));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPackageSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x71d46afb1865d711);
    schema = (schema ^ (uint64_t)sizeof(((UmiPackageSpec *)0)->package_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPackageSpec *)0)->staging_root)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPackageSpec *)0)->output_path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPackageSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPackageSpec *)0)->package_id) - 1U +
        8U +
        8U + sizeof(((UmiPackageSpec *)0)->staging_root) - 1U +
        8U + sizeof(((UmiPackageSpec *)0)->output_path) - 1U +
        8U;
}
static void UmiPackageSpecArchiveWrite(UmiArchiveWriter *writer, const UmiPackageSpec *value)
{
    UmiArchiveWriteText(writer, value->package_id, sizeof(value->package_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->format);
    UmiArchiveWriteText(writer, value->staging_root, sizeof(value->staging_root));
    UmiArchiveWriteText(writer, value->output_path, sizeof(value->output_path));
    UmiArchiveWriteSigned(writer, (int64_t)value->include_symbols);
}
static void UmiPackageSpecArchiveRead(UmiArchiveReader *reader, UmiPackageSpec *value)
{
    UmiArchiveReadText(reader, value->package_id, sizeof(value->package_id));
    value->format = (UmiPackageFormat)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->staging_root, sizeof(value->staging_root));
    UmiArchiveReadText(reader, value->output_path, sizeof(value->output_path));
    value->include_symbols = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiPackageSpecArchiveValidate(const UmiPackageSpec *value)
{
    return umi_package_spec_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_package_spec_archive_encode, umi_package_spec_archive_decode,
    UmiPackageSpec, UmiPackageSpecArchiveSchema, UmiPackageSpecArchiveBound, UmiPackageSpecArchiveWrite, UmiPackageSpecArchiveRead, UmiPackageSpecArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_package_spec_archive_decode_with_workspace, UmiPackageSpec,
    UmiPackageSpecArchiveSchema, UmiPackageSpecArchiveBound, UmiPackageSpecArchiveRead, UmiPackageSpecArchiveValidate)

/* Guard restoration with the previously reviewed encoded value. Domain field
 * decoding and validation remain here; the shared helper owns temporary memory. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_package_spec_archive_decode_if_unchanged, umi_package_spec_archive_encode,
    UmiPackageSpec, UmiPackageSpecArchiveSchema, UmiPackageSpecArchiveBound, UmiPackageSpecArchiveRead, UmiPackageSpecArchiveValidate)
