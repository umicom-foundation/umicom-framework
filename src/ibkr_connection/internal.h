/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Read-only IBKR session: an explicit Paper/Live selection is intent, not proof
 * of the environment running in TWS. No order-submission API exists here.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IBKR_CONNECTION_INTERNAL_H
#define UMICOM_IBKR_CONNECTION_INTERNAL_H
#include "umicom/broker_connectivity/connection.h"
#define UMI_IBKR_FRAME_LIMIT 65536U
#define UMI_IBKR_TX_LIMIT 8192U
/* Private injectable I/O permits deterministic failure tests. BUSY means no
 * progress. OK with a zero-byte read means EOF, never an empty application frame.
 * Context remains caller-owned; Close is called exactly once after Open. */
typedef struct UmiIbkrIo {
    void *context;
    UmiStatus (*Open)(void *, uint16_t);
    UmiStatus (*Ready)(void *);
    UmiStatus (*Read)(void *, void *, size_t, size_t *);
    UmiStatus (*Write)(void *, const void *, size_t, size_t *);
    void (*Close)(void *);
} UmiIbkrIo;
struct UmiIbkrConnection {
    UmiIbkrConnectionOptions options;
    UmiIbkrConnectionSnapshot snapshot;
    UmiIbkrIo io;
    void (*DestroyIo)(void *);
    bool ioOpened, opened, accountsReceived, nextIdReceived, pingPending;
    uint64_t startedAt, lastNow, pingAt;
    unsigned char rx[UMI_IBKR_FRAME_LIMIT+4U], tx[UMI_IBKR_TX_LIMIT];
    size_t rxSize, txSize;
};
UmiStatus UmiIbkrConnectionCreateWithIo(const UmiIbkrConnectionOptions *, const UmiIbkrIo *, UmiIbkrConnection **);
UmiStatus UmiIbkrNativeIo(UmiIbkrIo *outIo, void (**outDestroy)(void *));
UmiStatus UmiIbkrProcessFrame(UmiIbkrConnection *, const unsigned char *, size_t, uint64_t);
UmiStatus UmiIbkrQueueFields(UmiIbkrConnection *, const char *const *, size_t);
bool UmiIbkrText(const char *text, size_t capacity, bool allowEmpty);
bool UmiIbkrUnsigned(const char *, uint64_t *);
bool UmiIbkrDecimalText(const char *);
#endif
