/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/native_kernel_abi_report.h
 *
 * PURPOSE:
 *   Read one complete native Kernel ABI record on a hosted development system.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_NATIVE_KERNEL_ABI_REPORT_H
#define UMICOM_PLATFORM_NATIVE_KERNEL_ABI_REPORT_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_NATIVE_KERNEL_ABI_REPORT_MAX_BYTES 512U

typedef struct UmiNativeKernelAbiReport {
    uint16_t major;
    uint16_t minor;
    uint64_t wordBits;
    uint64_t byteOrder;
    uint64_t pageBytes;
    uint64_t copyLimit;
    uint64_t callLimit;
    uint64_t compiledFeatures;
} UmiNativeKernelAbiReport;

/* Parse only the exact frame emitted by the Kernel console's abi --record
 * command. Accept LF or CRLF line endings and require the closing line and
 * its newline. The frame schema version is separate from the ABI version.
 * Keep unknown compiled feature bits so a caller can review them. Admission
 * does not establish ABI compatibility, available handles or privileges.
 * Unsupported frame versions return UNAVAILABLE. Invalid or incomplete
 * records leave outReport unchanged; no view into the input is retained. */
UmiStatus UmiNativeKernelAbiReportParse(const char *bytes, size_t length,
    UmiNativeKernelAbiReport *outReport);

/* This blocking read takes an explicit absolute path in a trusted directory.
 * It reuses native bounded regular-file admission and never executes the
 * content, starts an emulator or changes the file. Use an existing worker
 * in interactive applications. Failure leaves outReport unchanged. */
UmiStatus UmiNativeKernelAbiReportLoad(const char *absolutePath,
    UmiNativeKernelAbiReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
