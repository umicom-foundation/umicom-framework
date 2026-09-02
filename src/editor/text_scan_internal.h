/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_scan_internal.h
 *
 * PURPOSE:
 *   Declare the text scan internal contract shared by Framework services and
 *   thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework internal editor scan ABI.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H
#define UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

/**
 * Provide the editor text scan count byte portable operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_text_scan_count_byte_portable(const unsigned char *bytes,
                                                   size_t byte_count,
                                                   unsigned char value);
/**
 * Provide the editor text scan count non ascii portable operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_text_scan_count_non_ascii_portable(
    const unsigned char *bytes,
    size_t byte_count);

#if defined(UMICOM_EDITOR_SCAN_X86_64_ASM)
/**
 * Provide the editor text scan count byte x86 64 operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_text_scan_count_byte_x86_64(const unsigned char *bytes,
                                                size_t byte_count,
                                                unsigned char value);
#elif defined(UMICOM_EDITOR_SCAN_AARCH64_ASM)
/**
 * Provide the editor text scan count byte aarch64 operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_text_scan_count_byte_aarch64(const unsigned char *bytes,
                                                 size_t byte_count,
                                                 unsigned char value);
#elif defined(UMICOM_EDITOR_SCAN_RISCV64_ASM)
/**
 * Provide the editor text scan count byte riscv64 operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_text_scan_count_byte_riscv64(const unsigned char *bytes,
                                                 size_t byte_count,
                                                 unsigned char value);
#endif

#endif /* UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H */
