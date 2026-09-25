/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/codec.c
 *
 * PURPOSE:
 *   Encode explicit bounded event fields in a portable, strictly checked text format rather than serialising C memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Schema UFIN1 is independent of enum storage width, padding and host byte
 * order. Text is hexadecimal; decimal numbers have one canonical spelling.
 * A damaged or newer record is rejected, never partly replayed. */
typedef struct EventWriter { char *buffer; size_t capacity, used; UmiStatus status; } EventWriter;
typedef struct EventReader { const char *cursor; UmiStatus status; } EventReader;

static void WriteToken(EventWriter *writer, const char *token)
{
    size_t length = strlen(token);
    if (writer->status != UMI_STATUS_OK) return;
    if (writer->used >= writer->capacity || length > writer->capacity - writer->used - 1U ||
        writer->capacity - writer->used - length < 2U) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    memcpy(writer->buffer + writer->used, token, length);
    writer->used += length;
    writer->buffer[writer->used++] = '|';
    writer->buffer[writer->used] = '\0';
}

static void WriteNumber(EventWriter *writer, int64_t number)
{
    char token[32];
    int written = snprintf(token, sizeof(token), "%" PRId64, number);
    if (written < 0 || (size_t)written >= sizeof(token)) { writer->status = UMI_STATUS_INTERNAL_ERROR; return; }
    WriteToken(writer, token);
}

static void WriteText(EventWriter *writer, const char *text)
{
    static const char HEX[] = "0123456789abcdef";
    char token[UMI_FINANCE_NAME_CAPACITY * 2U + 1U];
    size_t length = strlen(text);
    if (length >= UMI_FINANCE_NAME_CAPACITY) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    for (size_t index = 0U; index < length; ++index) {
        unsigned char byte = (unsigned char)text[index];
        token[index * 2U] = HEX[byte >> 4U];
        token[index * 2U + 1U] = HEX[byte & 15U];
    }
    token[length * 2U] = '\0';
    WriteToken(writer, token);
}

UmiStatus FinanceEncode(const UmiFinanceOperationCommand *command, char *buffer, size_t capacity)
{
    EventWriter writer = {buffer, capacity, 0U, UMI_STATUS_OK};
    const UmiFinancialId *ids[8];
    UmiStatus status = FinanceCommandValidate(command);
    if (status != UMI_STATUS_OK || buffer == NULL || capacity == 0U)
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    buffer[0] = '\0';
    if (command->expectedRevision > UMI_FINANCE_OPERATIONS_EVENTS) return UMI_STATUS_INVALID_ARGUMENT;
    ids[0] = &command->requestId; ids[1] = &command->actorId; ids[2] = &command->id;
    ids[3] = &command->referenceId; ids[4] = &command->periodId; ids[5] = &command->accountId;
    ids[6] = &command->participantId; ids[7] = &command->instrumentId;
    WriteToken(&writer, "UFIN1");
    WriteNumber(&writer, (int64_t)command->kind);
    WriteNumber(&writer, (int64_t)command->expectedRevision);
    for (size_t index = 0U; index < 8U; ++index) WriteText(&writer, ids[index]->value);
    WriteText(&writer, command->name);
    WriteText(&writer, command->currency.code);
    WriteNumber(&writer, (int64_t)command->scale);
    WriteNumber(&writer, (int64_t)command->accountClass);
    WriteNumber(&writer, (int64_t)command->side);
    WriteNumber(&writer, command->enabled ? 1 : 0);
    WriteNumber(&writer, command->date.year); WriteNumber(&writer, command->date.month); WriteNumber(&writer, command->date.day);
    WriteNumber(&writer, command->endDate.year); WriteNumber(&writer, command->endDate.month); WriteNumber(&writer, command->endDate.day);
    WriteNumber(&writer, command->amountMinor); WriteNumber(&writer, command->priceTicks); WriteNumber(&writer, command->lots);
    WriteNumber(&writer, command->minorPerTick); WriteNumber(&writer, command->unitsPerLot); WriteNumber(&writer, command->maxOrderLots);
    WriteNumber(&writer, (int64_t)command->lineCount);
    for (size_t index = 0U; index < command->lineCount; ++index) {
        WriteText(&writer, command->lines[index].accountId.value);
        WriteNumber(&writer, command->lines[index].debitMinor);
        WriteNumber(&writer, command->lines[index].creditMinor);
    }
    if (writer.status != UMI_STATUS_OK) buffer[0] = '\0';
    return writer.status;
}

static void ReadToken(EventReader *reader, char *token, size_t capacity)
{
    size_t used = 0U;
    if (reader->status != UMI_STATUS_OK) { token[0] = '\0'; return; }
    while (*reader->cursor != '\0' && *reader->cursor != '|') {
        if (used + 1U >= capacity) { reader->status = UMI_STATUS_PARSE_ERROR; token[0] = '\0'; return; }
        token[used++] = *reader->cursor++;
    }
    token[used] = '\0';
    if (*reader->cursor != '|') { reader->status = UMI_STATUS_PARSE_ERROR; return; }
    ++reader->cursor;
}

static int64_t ReadNumber(EventReader *reader)
{
    char token[32];
    const char *cursor;
    uint64_t value = 0U, limit;
    bool negative;
    ReadToken(reader, token, sizeof(token));
    if (reader->status != UMI_STATUS_OK) return 0;
    cursor = token;
    negative = *cursor == '-';
    if (negative) ++cursor;
    limit = negative ? (uint64_t)INT64_MAX + 1U : (uint64_t)INT64_MAX;
    if (*cursor == '\0' || (*cursor == '0' && (negative || cursor[1] != '\0')))
        reader->status = UMI_STATUS_PARSE_ERROR;
    while (*cursor != '\0' && reader->status == UMI_STATUS_OK) {
        unsigned digit;
        if (*cursor < '0' || *cursor > '9') { reader->status = UMI_STATUS_PARSE_ERROR; break; }
        digit = (unsigned)(*cursor - '0');
        if (value > (limit - digit) / 10U) { reader->status = UMI_STATUS_PARSE_ERROR; break; }
        value = value * 10U + digit;
        ++cursor;
    }
    if (reader->status != UMI_STATUS_OK) return 0;
    if (!negative) return (int64_t)value;
    if (value == (uint64_t)INT64_MAX + 1U) return INT64_MIN;
    return -(int64_t)value;
}

static int HexDigit(char digit)
{
    if (digit >= '0' && digit <= '9') return digit - '0';
    if (digit >= 'a' && digit <= 'f') return digit - 'a' + 10;
    return -1;
}

static void ReadText(EventReader *reader, char *text, size_t capacity)
{
    char token[UMI_FINANCE_NAME_CAPACITY * 2U + 1U];
    size_t length;
    ReadToken(reader, token, sizeof(token));
    if (reader->status != UMI_STATUS_OK) return;
    length = strlen(token);
    if ((length % 2U) != 0U || length / 2U >= capacity) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
    for (size_t index = 0U; index < length; index += 2U) {
        int high = HexDigit(token[index]), low = HexDigit(token[index + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
        text[index / 2U] = (char)(unsigned char)(high * 16 + low);
    }
    text[length / 2U] = '\0';
}

static int64_t ReadRange(EventReader *reader, int64_t minimum, int64_t maximum)
{
    int64_t number = ReadNumber(reader);
    if (number < minimum || number > maximum) { reader->status = UMI_STATUS_PARSE_ERROR; return minimum; }
    return number;
}

UmiStatus FinanceDecode(const char *text, UmiFinanceOperationCommand *out)
{
    UmiFinanceOperationCommand command;
    EventReader reader = {text, UMI_STATUS_OK};
    UmiFinancialId *ids[8];
    char token[16], canonical[FINANCE_RECORD_CAPACITY];
    size_t length = 0U;
    if (text == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (length < FINANCE_RECORD_CAPACITY && text[length] != '\0') ++length;
    if (length == FINANCE_RECORD_CAPACITY) return UMI_STATUS_PARSE_ERROR;
    UmiFinanceOperationCommandInit(&command);
    ReadToken(&reader, token, sizeof(token));
    if (strcmp(token, "UFIN1") != 0) return UMI_STATUS_PARSE_ERROR;
    command.kind = (UmiFinanceOperationKind)ReadRange(&reader, UMI_FINANCE_CREATE_ACCOUNT, UMI_FINANCE_REOPEN_REVIEW);
    command.expectedRevision = (uint64_t)ReadRange(&reader, 0, UMI_FINANCE_OPERATIONS_EVENTS);
    ids[0] = &command.requestId; ids[1] = &command.actorId; ids[2] = &command.id;
    ids[3] = &command.referenceId; ids[4] = &command.periodId; ids[5] = &command.accountId;
    ids[6] = &command.participantId; ids[7] = &command.instrumentId;
    for (size_t index = 0U; index < 8U; ++index) ReadText(&reader, ids[index]->value, sizeof(ids[index]->value));
    ReadText(&reader, command.name, sizeof(command.name));
    ReadText(&reader, command.currency.code, sizeof(command.currency.code));
    command.scale = (uint8_t)ReadRange(&reader, 0, 9);
    command.accountClass = (UmiAccountingAccountClass)ReadRange(&reader, UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_EXPENSE);
    command.side = (UmiSide)ReadRange(&reader, -1, 1);
    command.enabled = ReadRange(&reader, 0, 1) != 0;
    command.date.year = (int32_t)ReadRange(&reader, 0, 9999);
    command.date.month = (uint8_t)ReadRange(&reader, 0, 12); command.date.day = (uint8_t)ReadRange(&reader, 0, 31);
    command.endDate.year = (int32_t)ReadRange(&reader, 0, 9999);
    command.endDate.month = (uint8_t)ReadRange(&reader, 0, 12); command.endDate.day = (uint8_t)ReadRange(&reader, 0, 31);
    command.amountMinor = ReadNumber(&reader); command.priceTicks = ReadNumber(&reader); command.lots = ReadNumber(&reader);
    command.minorPerTick = ReadNumber(&reader); command.unitsPerLot = ReadNumber(&reader); command.maxOrderLots = ReadNumber(&reader);
    command.lineCount = (size_t)ReadRange(&reader, 0, UMI_FINANCE_OPERATIONS_JOURNAL_LINES);
    for (size_t index = 0U; index < command.lineCount; ++index) {
        ReadText(&reader, command.lines[index].accountId.value, sizeof(command.lines[index].accountId.value));
        command.lines[index].debitMinor = ReadNumber(&reader);
        command.lines[index].creditMinor = ReadNumber(&reader);
    }
    if (reader.status != UMI_STATUS_OK || *reader.cursor != '\0' ||
        FinanceEncode(&command, canonical, sizeof(canonical)) != UMI_STATUS_OK || strcmp(canonical, text) != 0)
        return UMI_STATUS_PARSE_ERROR;
    *out = command;
    return UMI_STATUS_OK;
}

bool FinanceSameCommand(const UmiFinanceOperationCommand *left, const UmiFinanceOperationCommand *right)
{
    UmiFinanceOperationCommand a = *left, b = *right;
    char encodedA[FINANCE_RECORD_CAPACITY], encodedB[FINANCE_RECORD_CAPACITY];
    a.expectedRevision = 0U; b.expectedRevision = 0U;
    return FinanceEncode(&a, encodedA, sizeof(encodedA)) == UMI_STATUS_OK &&
        FinanceEncode(&b, encodedB, sizeof(encodedB)) == UMI_STATUS_OK && strcmp(encodedA, encodedB) == 0;
}
