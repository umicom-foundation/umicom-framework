/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/internal.h
 *
 * PURPOSE:
 *   Share bounded canonical storage contracts within the provider connection service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROVIDER_CONNECTIONS_INTERNAL_H
#define UMICOM_PROVIDER_CONNECTIONS_INTERNAL_H
#include "umicom/provider_connections/connections.h"
#include "umicom/provider_connections/tasks.h"
#include "umicom/platform/threading.h"
#define UMI_PROVIDER_CONNECTION_WIRE_CAPACITY 4096U
#define UMI_PROVIDER_TASK_WIRE_CAPACITY 4096U
#define UMI_PROVIDER_CONNECTION_KEY_CAPACITY 192U
struct UmiProviderConnections {
    UmiDataServer *server;
    UmiMutex *mutex;
    char prefix[128];
    bool poisoned;
};
bool UmiProviderConnectionIdValid(const char *text, size_t capacity);
UmiStatus UmiProviderConnectionEncode(const UmiProviderConnection *connection,
    char *out, size_t capacity);
UmiStatus UmiProviderConnectionDecode(const char *wire, UmiProviderConnection *out);
UmiStatus UmiProviderConnectionIndexEncode(const UmiProviderConnectionSnapshot *snapshot,
    char *out, size_t capacity);
UmiStatus UmiProviderConnectionIndexDecode(const char *wire,
    UmiProviderConnectionSnapshot *out);
/* Private receipt persistence contract. Length excludes the terminating NUL.
 * Encoding needs room for that terminator; decoding accepts exactly length
 * bytes. Stage complete results and preserve output/length on every failure.
 * Unknown schema is NOT_IMPLEMENTED, malformed known records are PARSE_ERROR.
 * Output buffer and output length must not overlap. No authentication or I/O. */
UmiStatus UmiProviderTaskEncode(const UmiProviderTaskReceipt *receipt,
    char *out,size_t capacity,size_t *out_length);
UmiStatus UmiProviderTaskDecode(const char *wire,size_t length,
    UmiProviderTaskReceipt *out);
/* Finish is valid only after this caller successfully acquired Begin.
 * Failed rollback poisons the caller; these helpers never adopt another
 * transaction and never retry or reset a Data Server. */
UmiStatus UmiProviderStorageBegin(UmiDataServer *server,bool poisoned);
UmiStatus UmiProviderStorageFinish(UmiDataServer *server,bool *poisoned,UmiStatus status);
#endif
