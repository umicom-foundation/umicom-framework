/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_system.h
 *
 * PURPOSE:
 *   Own copied ComfyUI system and device observations without reserving memory or activating models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_SYSTEM_H
#define UMICOM_MEDIA_GENERATION_COMFY_SYSTEM_H
#include "umicom/provider_connections/connections.h"
#include "umicom/provider_connections/tasks.h"
#include "umicom/platform/cancellation.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_SYSTEM_MAX_BYTES (1024U * 1024U)
#define UMI_COMFY_SYSTEM_DEVICE_LIMIT 64U
typedef enum UmiComfySystemKnown {
    UMI_COMFY_SYSTEM_OS=1U, UMI_COMFY_SYSTEM_PYTHON=2U,
    UMI_COMFY_SYSTEM_PYTORCH=4U, UMI_COMFY_SYSTEM_VERSION=8U,
    UMI_COMFY_SYSTEM_EMBEDDED=16U, UMI_COMFY_SYSTEM_RAM_TOTAL=32U,
    UMI_COMFY_SYSTEM_RAM_FREE=64U
} UmiComfySystemKnown;
typedef enum UmiComfyDeviceKnown {
    UMI_COMFY_DEVICE_INDEX=1U, UMI_COMFY_DEVICE_VRAM_TOTAL=2U,
    UMI_COMFY_DEVICE_VRAM_FREE=4U, UMI_COMFY_DEVICE_TORCH_TOTAL=8U,
    UMI_COMFY_DEVICE_TORCH_FREE=16U
} UmiComfyDeviceKnown;
typedef struct UmiComfySystemSummary {
    char os[129],python_version[513],pytorch_version[129],comfyui_version[129];
    uint32_t known;
    bool embedded_python,ram_inconsistent;
    uint64_t ram_total,ram_free,connection_revision;
    size_t device_count,response_bytes;
    unsigned char connection_sha256[UMI_SHA256_BYTES],response_sha256[UMI_SHA256_BYTES];
} UmiComfySystemSummary;
typedef struct UmiComfyDeviceObservation {
    char name[513],type[65];
    uint32_t known;
    uint64_t index,vram_total,vram_free,torch_vram_total,torch_vram_free;
    bool vram_inconsistent,torch_vram_inconsistent;
} UmiComfyDeviceObservation;
typedef struct UmiComfySystemStats UmiComfySystemStats;
/* Observe host-supplied GET /system_stats response bytes. Require a system
 * object and devices array; device name/type are nonempty UTF-8 strings.
 * Other listed fields are optional: missing or JSON null is unknown, distinct
 * from known zero, false or empty version text. Non-null numeric values must
 * be integer tokens in 0..INT64_MAX; fractions, exponents and strings are not
 * coerced. Retain unknown device types and repeated device rows in wire order.
 *
 * Copy only named metadata. argv, package lists and other extension values
 * receive complete JSON validation but are not retained. Reject duplicate
 * decoded object keys at every depth. Limits: 1 MiB, 65536 JSON nodes, depth 32,
 * 64 devices; string byte limits follow the public arrays minus the terminator.
 *
 * The saved enabled ComfyUI loopback origin and nonzero revision label the
 * observation. They do not authenticate a response or establish freshness.
 * The host must verify transport/status separately. No I/O or provider action.
 * Keep inputs stable and initialize *out=NULL; every failure preserves it.
 * Cancellation is cooperative; bounded copy/hash work is not interruptible. */
UmiStatus UmiComfySystemStatsCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision,const void *bytes,size_t length,
    const UmiCancellationToken *cancel,UmiComfySystemStats **out);
void UmiComfySystemStatsDestroy(UmiComfySystemStats *stats);
/* Outputs are copied and unchanged on refusal. Unknown fields have zeroed
 * storage; check known bits before use. Inconsistent means both counters are
 * known and free exceeds total, not that the server or hardware is unhealthy.
 * RAM/VRAM/torch values may overlap; never sum them or treat free as reserved
 * capacity. No model-fit estimate, used-memory subtraction or device selection.
 * Keep the owner alive while reading; coordinate reads and destruction. */
UmiStatus UmiComfySystemStatsInspect(const UmiComfySystemStats *stats,UmiComfySystemSummary *out);
UmiStatus UmiComfySystemStatsDeviceAt(const UmiComfySystemStats *stats,size_t index,UmiComfyDeviceObservation *out);
/* BUSY means different saved content/revision. A match proves neither transport
 * identity nor that the captured counters are current. */
UmiStatus UmiComfySystemStatsCheckConnection(const UmiComfySystemStats *stats,
    const UmiProviderConnection *current,uint64_t current_revision);
#ifdef __cplusplus
}
#endif
#endif
