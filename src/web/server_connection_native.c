/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_connection_native.c
 * PURPOSE: Adapt one bounded local HTTP exchange to nonblocking Windows or POSIX sockets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "server_connection_internal.h"
#include "umicom/platform/clock.h"
#include <limits.h>
#include <string.h>
#if !defined(__EMSCRIPTEN__) && (defined(_WIN32) || defined(__unix__) || defined(__APPLE__))
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET WebSocket;
#define WEB_INVALID_SOCKET INVALID_SOCKET
static void SocketClose(WebSocket socket) { closesocket(socket); }
static int RetrySocket(void)
{
    int error = WSAGetLastError();
    return error == WSAEWOULDBLOCK || error == WSAEINTR;
}
static int Interrupted(void) { return WSAGetLastError() == WSAEINTR; }
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int WebSocket;
#define WEB_INVALID_SOCKET (-1)
static void SocketClose(WebSocket socket) { (void)close(socket); }
static int RetrySocket(void) { return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR; }
static int Interrupted(void) { return errno == EINTR; }
#endif
typedef struct NativeExchange
{
    WebSocket socket;
    UmiClock clock;
    uint64_t deadline, last_now;
    const UmiCancellationToken *cancel;
} NativeExchange;
static UmiStatus SocketPrepare(WebSocket socket, bool sending)
{
#ifdef _WIN32
    u_long enabled = 1UL;
/* Winsock takes its ioctl command as a signed long even though FIONBIO
 * is defined with an unsigned high bit. Convert explicitly at this Windows
 * API boundary, preserving the command bits and the non-inheritable socket
 * check; do not weaken conversion diagnostics for the rest of the module.
 * The superseded implementation is retained below for engineering review. */
#if 0
    if (ioctlsocket(socket, FIONBIO, &enabled) != 0 ||
        !SetHandleInformation((HANDLE)socket, HANDLE_FLAG_INHERIT, 0))
        return UMI_STATUS_IO_ERROR;
#endif
    /* Convert only the Windows command argument; both socket checks remain required. */
    if (ioctlsocket(socket, (long)FIONBIO, &enabled) != 0 ||
        !SetHandleInformation((HANDLE)socket, HANDLE_FLAG_INHERIT, 0))
        return UMI_STATUS_IO_ERROR;
    (void)sending;
#else
    if (socket < 0 || socket >= FD_SETSIZE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int flags = fcntl(socket, F_GETFL, 0), descriptor = fcntl(socket, F_GETFD, 0);
    if (flags < 0 || descriptor < 0 || fcntl(socket, F_SETFL, flags | O_NONBLOCK) < 0 ||
        fcntl(socket, F_SETFD, descriptor | FD_CLOEXEC) < 0)
        return UMI_STATUS_IO_ERROR;
#if defined(SO_NOSIGPIPE)
    if (sending)
    {
        int enabled = 1;
        if (setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled)) != 0)
            return UMI_STATUS_IO_ERROR;
    }
#elif !defined(MSG_NOSIGNAL)
    /* Do not change global SIGPIPE handlers in an embedding application. */
    if (sending)
        return UMI_STATUS_NOT_IMPLEMENTED;
#else
    (void)sending;
#endif
#endif
    return UMI_STATUS_OK;
}
static UmiStatus SetDeadline(NativeExchange *io, uint32_t milliseconds)
{
    uint64_t now = io->clock.monotonic_nanoseconds(&io->clock) / UINT64_C(1000000);
    if (now == 0U || now < io->last_now || now > UINT64_MAX - milliseconds)
        return UMI_STATUS_IO_ERROR;
    io->last_now = now;
    io->deadline = now + milliseconds;
    return UMI_STATUS_OK;
}
static UmiStatus WaitSocket(NativeExchange *io, bool writing)
{
    for (;;)
    {
        if (umi_cancellation_token_is_requested(io->cancel))
            return UMI_STATUS_CANCELLED;
        uint64_t now = io->clock.monotonic_nanoseconds(&io->clock) / UINT64_C(1000000);
        if (now == 0U || now < io->last_now)
            return UMI_STATUS_IO_ERROR;
        io->last_now = now;
        if (now >= io->deadline)
            return UMI_STATUS_TIMEOUT;
        uint64_t remaining = io->deadline - now;
        long slice = (long)(remaining > 50U ? 50U : remaining);
        struct timeval timeout = {0, slice * 1000L};
        fd_set ready;
        FD_ZERO(&ready);
        FD_SET(io->socket, &ready);
#ifdef _WIN32
        int selected = select(0, writing ? NULL : &ready, writing ? &ready : NULL, NULL, &timeout);
#else
        int selected =
            select(io->socket + 1, writing ? NULL : &ready, writing ? &ready : NULL, NULL, &timeout);
#endif
        if (selected > 0)
            return UMI_STATUS_OK;
        if (selected < 0 && !Interrupted())
            return UMI_STATUS_IO_ERROR;
    }
}
static UmiStatus ReadSocket(void *context, void *bytes, size_t capacity, size_t *out_bytes)
{
    NativeExchange *io = context;
    *out_bytes = 0U;
    if (capacity == 0U || capacity > (size_t)INT_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (;;)
    {
        UmiStatus status = WaitSocket(io, false);
        if (status != UMI_STATUS_OK)
            return status;
#ifdef _WIN32
        int count = recv(io->socket, bytes, (int)capacity, 0);
#else
        ssize_t count = recv(io->socket, bytes, capacity, 0);
#endif
        if (count > 0)
        {
            *out_bytes = (size_t)count;
            return UMI_STATUS_OK;
        }
        if (count == 0)
            return UMI_STATUS_UNAVAILABLE;
        if (!RetrySocket())
            return UMI_STATUS_IO_ERROR;
    }
}
static UmiStatus WriteSocket(void *context, const void *bytes, size_t length, size_t *out_bytes)
{
    NativeExchange *io = context;
    *out_bytes = 0U;
    if (length == 0U || length > (size_t)INT_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (;;)
    {
        UmiStatus status = WaitSocket(io, true);
        if (status != UMI_STATUS_OK)
            return status;
#ifdef _WIN32
        int count = send(io->socket, bytes, (int)length, 0);
#else
#ifdef MSG_NOSIGNAL
        ssize_t count = send(io->socket, bytes, length, MSG_NOSIGNAL);
#else
        ssize_t count = send(io->socket, bytes, length, 0);
#endif
#endif
        if (count > 0)
        {
            *out_bytes = (size_t)count;
            return UMI_STATUS_OK;
        }
        if (count == 0)
            return UMI_STATUS_UNAVAILABLE;
        if (!RetrySocket())
            return UMI_STATUS_IO_ERROR;
    }
}
UmiStatus WebNativeServeNext(UmiWebListener *listener, UmiWebService *service, size_t maximum,
                             uint32_t wait_ms, uint32_t exchange_ms, const UmiCancellationToken *cancel,
                             UmiWebExchangeResult *out)
{
    NativeExchange io = {0};
    io.socket = (WebSocket)listener->native_handle;
    io.clock = umi_clock_system();
    io.cancel = cancel;
    UmiStatus status = SocketPrepare(io.socket, false);
    if (status != UMI_STATUS_OK)
        return status;
    status = SetDeadline(&io, wait_ms);
    if (status != UMI_STATUS_OK)
        return status;
    WebSocket peer = WEB_INVALID_SOCKET;
    while (peer == WEB_INVALID_SOCKET)
    {
        status = WaitSocket(&io, false);
        if (status != UMI_STATUS_OK)
            return status;
        peer = accept(io.socket, NULL, NULL);
        if (peer == WEB_INVALID_SOCKET && !RetrySocket())
            return UMI_STATUS_IO_ERROR;
    }
    /* The accepted handle belongs only to this exchange. Every exit below
     * closes it; the reusable listening socket stays with its server. */
    io.socket = peer;
    status = SocketPrepare(peer, true);
    if (status == UMI_STATUS_OK)
        status = SetDeadline(&io, exchange_ms);
    if (status == UMI_STATUS_OK)
    {
        UmiWebConnectionIo transport = {&io, ReadSocket, WriteSocket};
        status = UmiWebConnectionProcess(service, &transport, maximum, cancel, out);
    }
    SocketClose(peer);
    return status;
}
#else
UmiStatus WebNativeServeNext(UmiWebListener *listener, UmiWebService *service, size_t maximum,
                             uint32_t wait_ms, uint32_t exchange_ms, const UmiCancellationToken *cancel,
                             UmiWebExchangeResult *out)
{
    (void)listener;
    (void)service;
    (void)maximum;
    (void)wait_ms;
    (void)exchange_ms;
    (void)cancel;
    (void)out;
    return UMI_STATUS_NOT_IMPLEMENTED;
}
#endif
