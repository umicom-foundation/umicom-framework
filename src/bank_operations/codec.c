/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/codec.c
 *
 * PURPOSE:
 *   Encode and validate portable, bounded banking event records without struct dumps.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <limits.h>
#include <string.h>

#define BANK_BINARY_CAPACITY 768U

typedef struct BankWriter {
    unsigned char bytes[BANK_BINARY_CAPACITY];
    size_t size;
    bool failed;
} BankWriter;

typedef struct BankReader {
    unsigned char bytes[BANK_BINARY_CAPACITY];
    size_t size;
    size_t position;
    bool failed;
} BankReader;

/* A byte-oriented format avoids ABI padding, endianness and sizeof(size_t).
 * Hex is a storage transport, not encryption or tamper-proof authentication. */
static void WriteNumber(BankWriter *writer, uint64_t value, size_t width)
{
    if (writer->size > BANK_BINARY_CAPACITY - width) { writer->failed = true; return; }
    for (size_t i = 0U; i < width; ++i) {
        writer->bytes[writer->size++] = (unsigned char)(value & UINT64_C(255));
        value >>= 8U;
    }
}

static uint64_t ReadNumber(BankReader *reader, size_t width)
{
    uint64_t value = 0U;
    if (reader->position > reader->size || width > reader->size - reader->position) {
        reader->failed = true; return 0U;
    }
    for (size_t i = 0U; i < width; ++i)
        value |= (uint64_t)reader->bytes[reader->position++] << (8U * i);
    return value;
}

static void WriteText(BankWriter *writer, const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    size_t length;
    if (end == NULL) { writer->failed = true; return; }
    length = (size_t)(end - text);
    if (length > 255U || writer->size + length + 1U > BANK_BINARY_CAPACITY) {
        writer->failed = true; return;
    }
    WriteNumber(writer, (uint64_t)length, 1U);
    memcpy(writer->bytes + writer->size, text, length);
    writer->size += length;
}

static void ReadText(BankReader *reader, char *text, size_t capacity)
{
    size_t length = (size_t)ReadNumber(reader, 1U);
    if (reader->failed || length >= capacity || reader->position > reader->size ||
        length > reader->size - reader->position ||
        memchr(reader->bytes + reader->position, 0, length) != NULL) {
        reader->failed = true; return;
    }
    memcpy(text, reader->bytes + reader->position, length);
    text[length] = '\0';
    reader->position += length;
}

UmiStatus BankEncode(const UmiBankAuditEvent *event, char *out, size_t capacity)
{
    static const char hex[] = "0123456789abcdef";
    BankWriter writer = {{0}, 0U, false};
    const UmiBankCommand *command;
    if (event == NULL || out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    command = &event->command;
    WriteNumber(&writer, UINT32_C(0x314f4255), 4U); /* UBO1: persistence schema, not an API suffix. */
    WriteNumber(&writer, event->revision, 8U);
    WriteNumber(&writer, (uint32_t)command->action, 4U);
    WriteNumber(&writer, event->actor.capabilities, 4U);
    WriteNumber(&writer, (uint32_t)command->state, 4U);
    WriteText(&writer, event->actor.id.value, sizeof event->actor.id.value);
    WriteText(&writer, command->requestId.value, sizeof command->requestId.value);
    WriteText(&writer, command->id.value, sizeof command->id.value);
    WriteText(&writer, command->ownerId.value, sizeof command->ownerId.value);
    WriteText(&writer, command->sourceAccountId.value, sizeof command->sourceAccountId.value);
    WriteText(&writer, command->destinationAccountId.value, sizeof command->destinationAccountId.value);
    WriteText(&writer, command->name, sizeof command->name);
    WriteText(&writer, command->amount.currency.code, sizeof command->amount.currency.code);
    WriteNumber(&writer, (uint64_t)command->amount.minor_units, 8U);
    WriteNumber(&writer, command->amount.scale, 1U);
    WriteNumber(&writer, (uint32_t)command->businessDate.year, 4U);
    WriteNumber(&writer, command->businessDate.month, 1U);
    WriteNumber(&writer, command->businessDate.day, 1U);
    WriteNumber(&writer, (uint64_t)command->timestampMillis, 8U);
    if (writer.failed || writer.size * 2U + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < writer.size; ++i) {
        out[2U * i] = hex[writer.bytes[i] >> 4U];
        out[2U * i + 1U] = hex[writer.bytes[i] & 15U];
    }
    out[writer.size * 2U] = '\0';
    return UMI_STATUS_OK;
}

static int HexDigit(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

UmiStatus BankDecode(const char *text, UmiBankAuditEvent *out)
{
    BankReader reader = {{0}, 0U, 0U, false};
    UmiBankAuditEvent event = {0};
    UmiBankCommand *command = &event.command;
    size_t length;
    uint64_t amount, year, timestamp, action, state;
    if (text == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    /* Repository values are bounded, terminated buffers supplied by Data Server. */
    length = strlen(text);
    if (length == 0U || length % 2U != 0U || length / 2U > BANK_BINARY_CAPACITY)
        return UMI_STATUS_PARSE_ERROR;
    reader.size = length / 2U;
    for (size_t i = 0U; i < reader.size; ++i) {
        int high = HexDigit(text[2U * i]), low = HexDigit(text[2U * i + 1U]);
        if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
        reader.bytes[i] = (unsigned char)(high * 16 + low);
    }
    if (ReadNumber(&reader, 4U) != UINT32_C(0x314f4255)) return UMI_STATUS_PARSE_ERROR;
    event.revision = ReadNumber(&reader, 8U);
    action = ReadNumber(&reader, 4U);
    event.actor.capabilities = (uint32_t)ReadNumber(&reader, 4U);
    state = ReadNumber(&reader, 4U);
    if (action < UMI_BANK_CUSTOMER_CREATE || action > UMI_BANK_RECONCILE || state > UMI_BANK_RECORD_CLOSED)
        return UMI_STATUS_PARSE_ERROR;
    command->action = (UmiBankAction)action;
    command->state = (UmiBankRecordState)state;
    ReadText(&reader, event.actor.id.value, sizeof event.actor.id.value);
    ReadText(&reader, command->requestId.value, sizeof command->requestId.value);
    ReadText(&reader, command->id.value, sizeof command->id.value);
    ReadText(&reader, command->ownerId.value, sizeof command->ownerId.value);
    ReadText(&reader, command->sourceAccountId.value, sizeof command->sourceAccountId.value);
    ReadText(&reader, command->destinationAccountId.value, sizeof command->destinationAccountId.value);
    ReadText(&reader, command->name, sizeof command->name);
    ReadText(&reader, command->amount.currency.code, sizeof command->amount.currency.code);
    amount = ReadNumber(&reader, 8U);
    command->amount.scale = (uint8_t)ReadNumber(&reader, 1U);
    year = ReadNumber(&reader, 4U);
    command->businessDate.month = (uint8_t)ReadNumber(&reader, 1U);
    command->businessDate.day = (uint8_t)ReadNumber(&reader, 1U);
    timestamp = ReadNumber(&reader, 8U);
    if (reader.failed || reader.position != reader.size || amount > INT64_MAX ||
        timestamp > INT64_MAX || year > INT32_MAX || event.revision == 0U ||
        event.revision > UMI_BANK_EVENT_CAPACITY) return UMI_STATUS_PARSE_ERROR;
    command->amount.minor_units = (int64_t)amount;
    command->businessDate.year = (int32_t)year;
    command->timestampMillis = (int64_t)timestamp;
    command->expectedRevision = event.revision - 1U;
    if (BankCommandValid(&event.actor, command) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *out = event;
    return UMI_STATUS_OK;
}

bool BankSameRequest(const UmiBankAuditEvent *event, const UmiBankActor *actor,
                     const UmiBankCommand *command)
{
    UmiBankAuditEvent left, right;
    char a[BANK_RECORD_TEXT_CAPACITY], b[BANK_RECORD_TEXT_CAPACITY];
    if (event == NULL || actor == NULL || command == NULL) return false;
    left = *event;
    right = (UmiBankAuditEvent){0};
    right.actor = *actor;
    right.command = *command;
    /* Capabilities can change between retries, but current authority was
     * checked before this comparison. Identity and financial payload cannot. */
    left.revision = right.revision = 0U;
    left.actor.capabilities = right.actor.capabilities = 0U;
    return BankEncode(&left, a, sizeof a) == UMI_STATUS_OK &&
        BankEncode(&right, b, sizeof b) == UMI_STATUS_OK && strcmp(a, b) == 0;
}
