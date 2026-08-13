/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_scan.c
 *
 * PURPOSE:
 *   Dispatch editor text scans to the selected C or assembly backend while
 *   preserving one public ABI and exactly equivalent results.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_scan.h"

#include "text_scan_internal.h"

#include <string.h>

uint64_t umi_editor_text_scan_count_byte(const unsigned char *bytes,
                                          size_t byte_count,
                                          unsigned char value)
{
    if (bytes == NULL || byte_count == 0U) return 0U;
#if defined(UMICOM_EDITOR_SCAN_X86_64_ASM)
    return umi_editor_text_scan_count_byte_x86_64(bytes, byte_count, value);
#elif defined(UMICOM_EDITOR_SCAN_AARCH64_ASM)
    return umi_editor_text_scan_count_byte_aarch64(bytes, byte_count, value);
#elif defined(UMICOM_EDITOR_SCAN_RISCV64_ASM)
    return umi_editor_text_scan_count_byte_riscv64(bytes, byte_count, value);
#else
    return umi_editor_text_scan_count_byte_portable(bytes, byte_count, value);
#endif
}

UmiStatus umi_editor_text_scan_profile(const char *bytes,
                                        size_t byte_count,
                                        UmiEditorTextScanProfile *out_profile)
{
    const unsigned char *unsigned_bytes = (const unsigned char *)bytes;
    if ((bytes == NULL && byte_count > 0U) || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_profile, 0, sizeof(*out_profile));
    out_profile->byte_count = (uint64_t)byte_count;
    out_profile->backend = umi_editor_text_scan_backend();
    if (byte_count == 0U) return UMI_STATUS_OK;
    out_profile->line_feed_count = umi_editor_text_scan_count_byte(
        unsigned_bytes, byte_count, (unsigned char)'\n');
    out_profile->carriage_return_count = umi_editor_text_scan_count_byte(
        unsigned_bytes, byte_count, (unsigned char)'\r');
    out_profile->tab_count = umi_editor_text_scan_count_byte(
        unsigned_bytes, byte_count, (unsigned char)'\t');
    out_profile->space_count = umi_editor_text_scan_count_byte(
        unsigned_bytes, byte_count, (unsigned char)' ');
    out_profile->non_ascii_count = umi_editor_text_scan_count_non_ascii_portable(
        unsigned_bytes, byte_count);
    return UMI_STATUS_OK;
}

UmiEditorTextScanBackend umi_editor_text_scan_backend(void)
{
#if defined(UMICOM_EDITOR_SCAN_X86_64_ASM)
    return UMI_EDITOR_TEXT_SCAN_X86_64_ASSEMBLY;
#elif defined(UMICOM_EDITOR_SCAN_AARCH64_ASM)
    return UMI_EDITOR_TEXT_SCAN_AARCH64_ASSEMBLY;
#elif defined(UMICOM_EDITOR_SCAN_RISCV64_ASM)
    return UMI_EDITOR_TEXT_SCAN_RISCV64_ASSEMBLY;
#else
    return UMI_EDITOR_TEXT_SCAN_PORTABLE_C;
#endif
}

const char *umi_editor_text_scan_backend_name(UmiEditorTextScanBackend backend)
{
    switch (backend) {
        case UMI_EDITOR_TEXT_SCAN_PORTABLE_C: return "portable-c";
        case UMI_EDITOR_TEXT_SCAN_X86_64_ASSEMBLY: return "x86-64-assembly";
        case UMI_EDITOR_TEXT_SCAN_AARCH64_ASSEMBLY: return "aarch64-assembly";
        case UMI_EDITOR_TEXT_SCAN_RISCV64_ASSEMBLY: return "riscv64-assembly";
        default: return "unknown";
    }
}
