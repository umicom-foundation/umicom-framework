/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/adapter_contract.h
 *
 * PURPOSE:
 *   Define a stable C ABI for native debuggers and Debug Adapter Protocol
 *   clients without exposing GDB, LLDB, CDB, Visual Studio, DAP transport or
 *   vendor-specific implementation types to applications.
 *
 * ARCHITECTURE:
 *   An adapter accepts bounded provider-neutral requests and returns bounded
 *   responses. Typed Framework models for threads, registers, memory and
 *   disassembly remain separate from transport parsing. This allows the same
 *   advanced debugger experience to operate over an in-process native adapter,
 *   a supervised DAP process, a remote development host or a deterministic test
 *   adapter.
 *
 * OWNERSHIP:
 *   The registry copies descriptors and function tables by value. Adapter
 *   instances remain owned by their provider. Request and response storage is
 *   caller-owned and valid for the duration of an invocation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_ADAPTER_CONTRACT_H
#define UMICOM_DEBUG_ADAPTER_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/debug/capabilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION 1U
#define UMI_DEBUG_ADAPTER_DEFAULT_CAPACITY 16U
#define UMI_DEBUG_ADAPTER_MAXIMUM_CAPACITY 256U
#define UMI_DEBUG_ADAPTER_ID_CAPACITY 128U
#define UMI_DEBUG_ADAPTER_LABEL_CAPACITY 256U
#define UMI_DEBUG_ADAPTER_KIND_CAPACITY 128U
#define UMI_DEBUG_ADAPTER_VERSION_CAPACITY 64U
#define UMI_DEBUG_ADAPTER_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_ADAPTER_THREAD_ID_CAPACITY 128U
#define UMI_DEBUG_ADAPTER_FRAME_ID_CAPACITY 128U
#define UMI_DEBUG_ADAPTER_REFERENCE_CAPACITY 512U
#define UMI_DEBUG_ADAPTER_EXPRESSION_CAPACITY 1024U
#define UMI_DEBUG_ADAPTER_PAYLOAD_CAPACITY 8192U
#define UMI_DEBUG_ADAPTER_MESSAGE_CAPACITY 512U

typedef enum UmiDebugAdapterOperation {
    UMI_DEBUG_ADAPTER_INITIALIZE = 1,
    UMI_DEBUG_ADAPTER_LAUNCH = 2,
    UMI_DEBUG_ADAPTER_ATTACH = 3,
    UMI_DEBUG_ADAPTER_CONFIGURATION_DONE = 4,
    UMI_DEBUG_ADAPTER_DISCONNECT = 5,
    UMI_DEBUG_ADAPTER_CONTINUE = 6,
    UMI_DEBUG_ADAPTER_PAUSE = 7,
    UMI_DEBUG_ADAPTER_STEP_OVER = 8,
    UMI_DEBUG_ADAPTER_STEP_INTO = 9,
    UMI_DEBUG_ADAPTER_STEP_OUT = 10,
    UMI_DEBUG_ADAPTER_STEP_BACK = 11,
    UMI_DEBUG_ADAPTER_REVERSE_CONTINUE = 12,
    UMI_DEBUG_ADAPTER_RESTART_FRAME = 13,
    UMI_DEBUG_ADAPTER_THREADS = 14,
    UMI_DEBUG_ADAPTER_STACK_TRACE = 15,
    UMI_DEBUG_ADAPTER_SCOPES = 16,
    UMI_DEBUG_ADAPTER_VARIABLES = 17,
    UMI_DEBUG_ADAPTER_EVALUATE = 18,
    UMI_DEBUG_ADAPTER_SET_VARIABLE = 19,
    UMI_DEBUG_ADAPTER_SET_EXPRESSION = 20,
    UMI_DEBUG_ADAPTER_SET_SOURCE_BREAKPOINTS = 21,
    UMI_DEBUG_ADAPTER_SET_FUNCTION_BREAKPOINTS = 22,
    UMI_DEBUG_ADAPTER_SET_DATA_BREAKPOINTS = 23,
    UMI_DEBUG_ADAPTER_SET_INSTRUCTION_BREAKPOINTS = 24,
    UMI_DEBUG_ADAPTER_SET_EXCEPTION_BREAKPOINTS = 25,
    UMI_DEBUG_ADAPTER_MODULES = 26,
    UMI_DEBUG_ADAPTER_READ_MEMORY = 27,
    UMI_DEBUG_ADAPTER_WRITE_MEMORY = 28,
    UMI_DEBUG_ADAPTER_DISASSEMBLE = 29,
    UMI_DEBUG_ADAPTER_REGISTERS = 30,
    UMI_DEBUG_ADAPTER_CANCEL = 31
} UmiDebugAdapterOperation;

typedef uint32_t UmiDebugAdapterFlags;
enum {
    UMI_DEBUG_ADAPTER_ENABLED = 1U << 0,
    UMI_DEBUG_ADAPTER_LOCAL = 1U << 1,
    UMI_DEBUG_ADAPTER_REMOTE = 1U << 2,
    UMI_DEBUG_ADAPTER_SUPERVISED_PROCESS = 1U << 3,
    UMI_DEBUG_ADAPTER_REQUIRES_WORKSPACE_TRUST = 1U << 4,
    UMI_DEBUG_ADAPTER_EXPERIMENTAL = 1U << 5,
    UMI_DEBUG_ADAPTER_THREAD_SAFE = 1U << 6,
    UMI_DEBUG_ADAPTER_ALL_FLAGS = (1U << 7) - 1U
};

typedef struct UmiDebugAdapterCancellation {
    uint32_t struct_size;
    uint32_t api_version;
    int (*is_cancelled)(void *user_data);
    void *user_data;
} UmiDebugAdapterCancellation;

/**
 * Generic invocation envelope. The payload is adapter-neutral data encoded by
 * the relevant Framework/DAP bridge. Common identifiers remain typed fields so
 * adapters do not need to parse transport data for routine routing decisions.
 */
typedef struct UmiDebugAdapterRequest {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t request_id;
    UmiDebugAdapterOperation operation;
    char session_id[UMI_DEBUG_ADAPTER_SESSION_ID_CAPACITY];
    char thread_id[UMI_DEBUG_ADAPTER_THREAD_ID_CAPACITY];
    char frame_id[UMI_DEBUG_ADAPTER_FRAME_ID_CAPACITY];
    char reference[UMI_DEBUG_ADAPTER_REFERENCE_CAPACITY];
    char expression[UMI_DEBUG_ADAPTER_EXPRESSION_CAPACITY];
    uint64_t address;
    int64_t offset;
    size_t count;
    uint32_t flags;
    size_t payload_length;
    unsigned char payload[UMI_DEBUG_ADAPTER_PAYLOAD_CAPACITY];
} UmiDebugAdapterRequest;

typedef struct UmiDebugAdapterResponse {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t request_id;
    UmiDebugAdapterOperation operation;
    UmiStatus status;
    uint64_t sequence;
    uint64_t revision;
    size_t payload_length;
    int complete;
    int cancelled;
    int stale;
    char message[UMI_DEBUG_ADAPTER_MESSAGE_CAPACITY];
    unsigned char payload[UMI_DEBUG_ADAPTER_PAYLOAD_CAPACITY];
} UmiDebugAdapterResponse;

typedef struct UmiDebugAdapterFunctions {
    uint32_t struct_size;
    uint32_t api_version;
    UmiStatus (*invoke)(
        void *instance,
        const UmiDebugAdapterRequest *request,
        const UmiDebugAdapterCancellation *cancellation,
        UmiDebugAdapterResponse *out_response);
    UmiStatus (*cancel)(void *instance, uint64_t request_id);
    UmiStatus (*health)(void *instance, char *out_message,
                        size_t message_capacity);
} UmiDebugAdapterFunctions;

typedef struct UmiDebugAdapterDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DEBUG_ADAPTER_ID_CAPACITY];
    char label[UMI_DEBUG_ADAPTER_LABEL_CAPACITY];
    char debugger_kind[UMI_DEBUG_ADAPTER_KIND_CAPACITY];
    char implementation_version[UMI_DEBUG_ADAPTER_VERSION_CAPACITY];
    uint64_t capabilities;
    UmiDebugAdapterFlags flags;
    int32_t priority;
    uint64_t sequence;
    uint64_t revision;
    void *instance;
    UmiDebugAdapterFunctions functions;
} UmiDebugAdapterDescriptor;

typedef struct UmiDebugAdapterRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t adapter_count;
    size_t enabled_count;
    size_t local_count;
    size_t remote_count;
    uint64_t revision;
} UmiDebugAdapterRegistrySnapshot;

typedef struct UmiDebugAdapterRegistry UmiDebugAdapterRegistry;

UmiStatus umi_debug_adapter_registry_create(
    size_t initial_capacity,
    UmiDebugAdapterRegistry **out_registry);
void umi_debug_adapter_registry_destroy(UmiDebugAdapterRegistry *registry);
UmiStatus umi_debug_adapter_registry_clear(UmiDebugAdapterRegistry *registry);
UmiStatus umi_debug_adapter_registry_register(
    UmiDebugAdapterRegistry *registry,
    const UmiDebugAdapterDescriptor *descriptor);
UmiStatus umi_debug_adapter_registry_unregister(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id);
UmiStatus umi_debug_adapter_registry_set_enabled(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    int enabled);
UmiStatus umi_debug_adapter_registry_find(
    const UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    UmiDebugAdapterDescriptor *out_descriptor);
UmiStatus umi_debug_adapter_registry_at(
    const UmiDebugAdapterRegistry *registry,
    size_t index,
    UmiDebugAdapterDescriptor *out_descriptor);
UmiStatus umi_debug_adapter_registry_select(
    const UmiDebugAdapterRegistry *registry,
    const char *debugger_kind,
    uint64_t required_capabilities,
    int allow_remote,
    UmiDebugAdapterDescriptor *out_descriptor);
UmiStatus umi_debug_adapter_registry_invoke(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response);
UmiStatus umi_debug_adapter_registry_cancel(
    UmiDebugAdapterRegistry *registry,
    const char *adapter_id,
    uint64_t request_id);
UmiStatus umi_debug_adapter_registry_snapshot(
    const UmiDebugAdapterRegistry *registry,
    UmiDebugAdapterRegistrySnapshot *out_snapshot);
size_t umi_debug_adapter_registry_count(
    const UmiDebugAdapterRegistry *registry);
uint64_t umi_debug_adapter_registry_revision(
    const UmiDebugAdapterRegistry *registry);
uint64_t umi_debug_adapter_operation_required_capability(
    UmiDebugAdapterOperation operation);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_ADAPTER_CONTRACT_H */
