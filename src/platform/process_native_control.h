/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_native_control.h
 *
 * PURPOSE:
 *   Keep borrowed native process handles inside the Framework supervisor lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROCESS_NATIVE_CONTROL_H
#define UMICOM_PROCESS_NATIVE_CONTROL_H
#include "umicom/platform/process.h"
/* The runner owns this handle. The observer publishes it under its own lock;
 * a NULL notification revokes it before the runner closes it. Never retain a
 * bare handle after revocation, reopen a PID, or expose it to application code.
 * Windows supplies a process HANDLE; other platforms do not supply a lease. */
typedef void (*UmiProcessNativeObserver)(void *nativeHandle, void *context);
UmiStatus UmiProcessExecuteSupervised(const UmiProcessRequest *request,
    UmiProcessLifetime lifetime, UmiProcessResultObserver observer,
    UmiProcessNativeObserver nativeObserver, void *context, UmiProcessResult *outResult);
UmiStatus UmiProcessActivateNativeWindow(void *nativeHandle);
#endif
