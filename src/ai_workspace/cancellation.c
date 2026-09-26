/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/cancellation.c
 *
 * PURPOSE:
 *   Keep cross-thread cancellation behind an opaque C ABI rather than exposing C atomic layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/workspace.h"
#include <stdatomic.h>
#include <stdlib.h>

struct UmiAiWorkspaceCancellation { atomic_bool requested; };
UmiStatus UmiAiWorkspaceCancellationCreate(UmiAiWorkspaceCancellation **outCancellation)
{
    UmiAiWorkspaceCancellation *cancellation;
    if (outCancellation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCancellation = NULL; cancellation = malloc(sizeof(*cancellation));
    if (cancellation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&cancellation->requested, false); *outCancellation = cancellation;
    return UMI_STATUS_OK;
}
void UmiAiWorkspaceCancellationRequest(UmiAiWorkspaceCancellation *cancellation)
{ if (cancellation != NULL) atomic_store(&cancellation->requested, true); }
void UmiAiWorkspaceCancellationReset(UmiAiWorkspaceCancellation *cancellation)
{ if (cancellation != NULL) atomic_store(&cancellation->requested, false); }
bool UmiAiWorkspaceCancellationIsRequested(const UmiAiWorkspaceCancellation *cancellation)
{ return cancellation != NULL && atomic_load(&cancellation->requested); }
void UmiAiWorkspaceCancellationDestroy(UmiAiWorkspaceCancellation *cancellation)
{ free(cancellation); }
