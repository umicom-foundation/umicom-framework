/*-----------------------------------------------------------------------------
 * Umicom Framework internal editor scan ABI.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H
#define UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

uint64_t umi_editor_text_scan_count_byte_portable(const unsigned char *bytes,
                                                   size_t byte_count,
                                                   unsigned char value);
uint64_t umi_editor_text_scan_count_non_ascii_portable(
    const unsigned char *bytes,
    size_t byte_count);

#if defined(UMICOM_EDITOR_SCAN_X86_64_ASM)
uint64_t umi_editor_text_scan_count_byte_x86_64(const unsigned char *bytes,
                                                size_t byte_count,
                                                unsigned char value);
#elif defined(UMICOM_EDITOR_SCAN_AARCH64_ASM)
uint64_t umi_editor_text_scan_count_byte_aarch64(const unsigned char *bytes,
                                                 size_t byte_count,
                                                 unsigned char value);
#elif defined(UMICOM_EDITOR_SCAN_RISCV64_ASM)
uint64_t umi_editor_text_scan_count_byte_riscv64(const unsigned char *bytes,
                                                 size_t byte_count,
                                                 unsigned char value);
#endif

#endif /* UMICOM_EDITOR_TEXT_SCAN_INTERNAL_H */
