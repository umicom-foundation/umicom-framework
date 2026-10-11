/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/instruction_inspection.h
 * PURPOSE: Capture bounded instruction pages for the selected paused frame without changing debugger registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_INSTRUCTION_INSPECTION_H
#define UMICOM_DEBUG_RUNTIME_INSTRUCTION_INSPECTION_H
#include "umicom/debug/selection.h"
#include "umicom/debug_runtime/connection_identity.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DEBUG_INSTRUCTION_PAGE_LIMIT 64U
#define UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT INT64_C(9007199254740991)
    typedef struct UmiDebugInstructionRecord
    {
        char address[128], bytes[256], text[1024], symbol[256], presentation_hint[64];
        UmiDebugRuntimeSource source;
        uint64_t line, column, end_line, end_column;
        int has_source, line_known, column_known, end_line_known, end_column_known;
    } UmiDebugInstructionRecord;
    typedef struct UmiDebugInstructionPage
    {
        size_t count;
        UmiDebugInstructionRecord items[UMI_DEBUG_INSTRUCTION_PAGE_LIMIT];
    } UmiDebugInstructionPage;
    typedef struct UmiDebugInstructionContext
    {
        UmiDebugConnectionIdentity connection;
        uint64_t revision;
        uint32_t thread_id, frame_id;
        char reference[128];
    } UmiDebugInstructionContext;
    typedef struct UmiDebugInstructionTarget UmiDebugInstructionTarget;
    /** Decode an entire successful disassemble response into an owned-by-value page.
 * requested must be 1..64 and the adapter must return exactly that many entries.
 * Required address/instruction and recognized optional fields are validated before
 * publishing any output. Unknown hints remain display text. Invalid filler rows
 * are retained, not treated as executable instructions. A missing location inherits
 * the preceding location within this page, as described by DAP.
 * Source paths, symbols and instruction bytes remain descriptive text only.
 * Output is unchanged on any failure. Allocate the page on the heap. */
    UmiStatus UmiDebugInstructionPageDecode(const char *json, uint32_t requested,
                                            UmiDebugInstructionPage *out);
    /** Read a signed decimal byte/instruction offset within the exact JSON integer range.
 * An optional leading '-' is accepted; whitespace, '+', exponents and hexadecimal
 * spelling are rejected. Failure preserves out. No adapter request occurs. */
    UmiStatus UmiDebugInstructionOffsetRead(const char *text, int64_t *out);
    /** Compare captured instruction contexts without contacting an adapter.
 * Both contexts must contain terminated identity/reference strings. A matching
 * context is descriptive evidence, not authority to execute a debugger command. */
    int UmiDebugInstructionContextEqual(const UmiDebugInstructionContext *left,
                                        const UmiDebugInstructionContext *right);
    /** Capture the selected paused frame and explicitly request its instruction reference.
 * The workspace must belong to platform's service and agree with its active frame.
 * One stackTrace request asks for the frame's existing stack position; an adapter
 * that omits its instructionPointerReference cannot provide this capture.
 * Capability, workspace generations, connection and pending events are checked
 * before and after the response. No selection, registry, memory or source changes
 * occur. Failure clears out. Owner-thread access only, with timeout_ms > 0. */
    UmiStatus UmiDebugInstructionTargetCapture(UmiDebugRuntimePlatform *platform,
                                               UmiDebugWorkspace *workspace, uint32_t timeout_ms,
                                               UmiDebugInstructionTarget **out);
    /** Release a captured frame target; accepts NULL. No adapter request occurs. */
    void UmiDebugInstructionTargetDestroy(UmiDebugInstructionTarget *target);
    /** Copy descriptive context that remains readable after the debugger closes.
 * Failure preserves out; the copied strings never become executable code. */
    UmiStatus UmiDebugInstructionTargetRead(const UmiDebugInstructionTarget *target,
                                            UmiDebugInstructionContext *out);
    /** Request one explicit page at signed byte and instruction offsets from the captured reference.
 * Count is 1..64. Both offsets stay within the exact JSON integer range above;
 * offset units are independent and are interpreted by the adapter, not added as addresses.
 * The same paused frame, workspace generations and connection must remain current.
 * Pending events return BUSY and remain queued. A successful reply is copied only
 * after rechecking all identities. No retries, evaluation, memory writes, process
 * continuation or registry replacement occur. Failure preserves out. */
    UmiStatus UmiDebugInstructionPageRead(UmiDebugRuntimePlatform *platform,
                                          UmiDebugWorkspace *workspace,
                                          const UmiDebugInstructionTarget *target,
                                          int64_t byte_offset, int64_t instruction_offset,
                                          uint32_t count, int resolve_symbols, uint32_t timeout_ms,
                                          UmiDebugInstructionPage *out);
#ifdef __cplusplus
}
#endif
#endif
