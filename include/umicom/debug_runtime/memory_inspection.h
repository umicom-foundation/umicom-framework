/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/memory_inspection.h
 * PURPOSE: Capture bounded memory from an owned variable at an unchanged native stop.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_MEMORY_INSPECTION_H
#define UMICOM_DEBUG_RUNTIME_MEMORY_INSPECTION_H
#include "umicom/debug_runtime/variable_inspection.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DEBUG_MEMORY_CAPTURE_CAPACITY 4096U
#define UMI_DEBUG_MEMORY_OFFSET_LIMIT INT64_C(9007199254740991)
    typedef struct UmiDebugMemoryCapture UmiDebugMemoryCapture;
    typedef struct UmiDebugMemoryBytes
    {
        char address[128];
        unsigned char bytes[UMI_DEBUG_MEMORY_CAPTURE_CAPACITY];
        size_t count;
        uint64_t unreadable;
    } UmiDebugMemoryBytes;
    /** Decode a complete readMemory envelope body. Address is an unsigned decimal
 * or 0x-prefixed hexadecimal string; data is canonical padded base64. A missing
 * body returns NOT_FOUND. Short data is retained without inventing missing bytes.
 * unreadable can extend beyond the requested range. Unknown extensions are
 * allowed; duplicate recognized fields, invalid text and excessive data fail.
 * requested must be 1..4096. out is unchanged on failure. */
    UmiStatus UmiDebugMemoryDecode(const char *json, uint32_t requested, UmiDebugMemoryBytes *out);
    /** Owner-thread availability check; never launches, sends, evaluates or writes.
 * A variable needs a memoryReference but need not have expandable children.
 * Capture variables using Inspect scope/children to retain adapter references;
 * the compatibility root registry does not store memoryReference. */
    UmiStatus UmiDebugRuntimeCheckMemory(UmiDebugRuntimePlatform *platform, UmiDebugWorkspace *workspace,
                                         const UmiDebugVariableTarget *target);
    /** Send one bounded read only after checking capability, owner, current stopped
 * frame and target generations. Pending events before/after the reply return
 * BUSY and stay queued for the host. Caller owns *out on success; otherwise NULL.
 * offset is signed and limited to the exact JSON integer range above. No
 * automatic retries, expression evaluation, memory writes or registry changes.
 * The owner thread may block for timeout_ms; do not access these owners in parallel. */
    UmiStatus UmiDebugRuntimeInspectMemory(UmiDebugRuntimePlatform *platform, UmiDebugWorkspace *workspace,
                                           const UmiDebugVariableTarget *target, int64_t offset,
                                           uint32_t count, uint32_t timeout_ms, UmiDebugMemoryCapture **out);
    void UmiDebugMemoryCaptureDestroy(UmiDebugMemoryCapture *capture);
    /** Copy retained bytes independently of the adapter and workspace lifetimes. */
    UmiStatus UmiDebugMemoryCaptureRead(const UmiDebugMemoryCapture *capture, UmiDebugMemoryBytes *out);
    /** Report whether the captured selection/generations are still current. This
 * does not poll the adapter or refresh the bytes. Retained bytes remain readable
 * after BUSY; another live read must go through InspectMemory. */
    UmiStatus UmiDebugMemoryCaptureValidate(const UmiDebugMemoryCapture *capture,
                                            UmiDebugWorkspace *workspace);
    /** Format a selectable hex/ASCII report without assuming pointer width or
 * interpreting target byte order. Row offsets are relative to the returned
 * address. required includes the terminating NUL; out may be NULL only for a
 * zero-capacity sizing call. Insufficient capacity leaves out unchanged. */
    UmiStatus UmiDebugMemoryCaptureFormat(const UmiDebugMemoryCapture *capture, char *out, size_t capacity,
                                          size_t *required);
#ifdef __cplusplus
}
#endif
#endif
