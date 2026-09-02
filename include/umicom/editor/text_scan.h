/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/text_scan.h
 *
 * PURPOSE:
 *   Expose portable text scanning with optional architecture-specific assembly
 *   acceleration selected by the Framework build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TEXT_SCAN_H
#define UMICOM_EDITOR_TEXT_SCAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named editor text scan backend values accepted by this public contract.
 */
typedef enum UmiEditorTextScanBackend {
    UMI_EDITOR_TEXT_SCAN_PORTABLE_C = 0,
    UMI_EDITOR_TEXT_SCAN_X86_64_ASSEMBLY = 1,
    UMI_EDITOR_TEXT_SCAN_AARCH64_ASSEMBLY = 2,
    UMI_EDITOR_TEXT_SCAN_RISCV64_ASSEMBLY = 3
} UmiEditorTextScanBackend;

/**
 * Represent the editor text scan profile data shared with callers of this public contract.
 */
typedef struct UmiEditorTextScanProfile {
    uint64_t byte_count;
    uint64_t line_feed_count;
    uint64_t carriage_return_count;
    uint64_t tab_count;
    uint64_t space_count;
    uint64_t non_ascii_count;
    UmiEditorTextScanBackend backend;
} UmiEditorTextScanProfile;

/**
 * Provide the editor text scan count byte operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_text_scan_count_byte(const unsigned char *bytes,
                                          size_t byte_count,
                                          unsigned char value);
/**
 * Provide the editor text scan profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_text_scan_profile(const char *bytes,
                                        size_t byte_count,
                                        UmiEditorTextScanProfile *out_profile);
/**
 * Provide the editor text scan backend operation used by this module and its client
 * applications.
 */
UmiEditorTextScanBackend umi_editor_text_scan_backend(void);
/**
 * Provide the editor text scan backend name operation used by this module and its client
 * applications.
 */
const char *umi_editor_text_scan_backend_name(UmiEditorTextScanBackend backend);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_TEXT_SCAN_H */
