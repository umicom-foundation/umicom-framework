/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/scan.c
 *
 * PURPOSE:
 *   Implement the scan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal scanner dispatch.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/scan.h"
#include "scan_internal.h"

uint64_t umi_terminal_scan_count_byte(const unsigned char *bytes,
                                      size_t byte_count,
                                      unsigned char value)
{
    if (bytes == NULL || byte_count == 0U) return 0U;
#if defined(UMICOM_TERMINAL_SCAN_X86_64_ASM)
    return umi_terminal_scan_count_byte_x86_64(bytes, byte_count, value);
#elif defined(UMICOM_TERMINAL_SCAN_AARCH64_ASM)
    return umi_terminal_scan_count_byte_aarch64(bytes, byte_count, value);
#elif defined(UMICOM_TERMINAL_SCAN_RISCV64_ASM)
    return umi_terminal_scan_count_byte_riscv64(bytes, byte_count, value);
#else
    return umi_terminal_scan_count_byte_portable(bytes, byte_count, value);
#endif
}

uint64_t umi_terminal_scan_line_count(const char *text, size_t byte_count)
{
    uint64_t line_feeds;
    if (text == NULL || byte_count == 0U) return 0U;
    line_feeds = umi_terminal_scan_count_byte((const unsigned char *)text,
                                               byte_count,
                                               (unsigned char)'\n');
    return line_feeds + (text[byte_count - 1U] == '\n' ? 0U : 1U);
}

UmiTerminalScanBackend umi_terminal_scan_backend(void)
{
#if defined(UMICOM_TERMINAL_SCAN_X86_64_ASM)
    return UMI_TERMINAL_SCAN_X86_64_ASSEMBLY;
#elif defined(UMICOM_TERMINAL_SCAN_AARCH64_ASM)
    return UMI_TERMINAL_SCAN_AARCH64_ASSEMBLY;
#elif defined(UMICOM_TERMINAL_SCAN_RISCV64_ASM)
    return UMI_TERMINAL_SCAN_RISCV64_ASSEMBLY;
#else
    return UMI_TERMINAL_SCAN_PORTABLE_C;
#endif
}

const char *umi_terminal_scan_backend_name(UmiTerminalScanBackend backend)
{
    switch (backend) {
        case UMI_TERMINAL_SCAN_PORTABLE_C: return "portable-c";
        case UMI_TERMINAL_SCAN_X86_64_ASSEMBLY: return "x86-64-assembly";
        case UMI_TERMINAL_SCAN_AARCH64_ASSEMBLY: return "aarch64-assembly";
        case UMI_TERMINAL_SCAN_RISCV64_ASSEMBLY: return "riscv64-assembly";
        default: return "unknown";
    }
}
