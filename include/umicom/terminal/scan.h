/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/scan.h
 *
 * PURPOSE:
 *   Provide one portable C ABI for terminal transcript byte scanning, with an
 *   optional audited assembly implementation selected by CMake.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_SCAN_H
#define UMICOM_TERMINAL_SCAN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiTerminalScanBackend {
    UMI_TERMINAL_SCAN_PORTABLE_C = 0,
    UMI_TERMINAL_SCAN_X86_64_ASSEMBLY = 1,
    UMI_TERMINAL_SCAN_AARCH64_ASSEMBLY = 2,
    UMI_TERMINAL_SCAN_RISCV64_ASSEMBLY = 3
} UmiTerminalScanBackend;

uint64_t umi_terminal_scan_count_byte(const unsigned char *bytes,
                                      size_t byte_count,
                                      unsigned char value);
uint64_t umi_terminal_scan_line_count(const char *text, size_t byte_count);
UmiTerminalScanBackend umi_terminal_scan_backend(void);
const char *umi_terminal_scan_backend_name(UmiTerminalScanBackend backend);

#ifdef __cplusplus
}
#endif
#endif
