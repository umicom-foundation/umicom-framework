/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/archive.c
 * PURPOSE: Reuse bounded value encoding for complete planning boards, never serializing object padding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "board_internal.h"
#include "umicom/kanban/archive.h"
#include "../base/value_archive_internal.h"
#include <stdlib.h>
#define KANBAN_SCHEMA UINT64_C(0x554d494b414e4201)
/* Boards using Blocked require an extended schema. Legacy boards still write
 * their original schema so older applications can continue reading them. */
#define KANBAN_WORKFLOW_SCHEMA UINT64_C(0x554d494b414e4202)
/* A developer directory extends the envelope only when records exist. */
#define KANBAN_DEVELOPERS_SCHEMA UINT64_C(0x554d494b414e4203)
#define WRITE_TEXT(writer, object, field) UmiArchiveWriteText(writer, (object)->field, sizeof((object)->field))
#define READ_TEXT(reader, object, field) UmiArchiveReadText(reader, (object)->field, sizeof((object)->field))
static void WriteCard(UmiArchiveWriter *writer, const UmiKanbanCard *card)
{
    WRITE_TEXT(writer, card, id); WRITE_TEXT(writer, card, title);
    WRITE_TEXT(writer, card, description); WRITE_TEXT(writer, card, assignee);
    UmiArchiveWriteUnsigned(writer, (uint64_t)card->priority);
    UmiArchiveWriteUnsigned(writer, card->due_unix_seconds);
    WRITE_TEXT(writer, card, blocked_reason);
    UmiArchiveWriteUnsigned(writer, card->label_count);
    for (size_t i = 0U; i < card->label_count; ++i)
        UmiArchiveWriteText(writer, card->labels[i], sizeof(card->labels[i]));
    UmiArchiveWriteUnsigned(writer, card->check_count);
    for (size_t i = 0U; i < card->check_count; ++i) {
        UmiArchiveWriteText(writer, card->checks[i].text, sizeof(card->checks[i].text));
        UmiArchiveWriteUnsigned(writer, (uint64_t)card->checks[i].complete);
    }
    UmiArchiveWriteUnsigned(writer, card->dependency_count);
    for (size_t i = 0U; i < card->dependency_count; ++i)
        UmiArchiveWriteText(writer, card->dependencies[i], sizeof(card->dependencies[i]));
}
static void ReadCard(UmiArchiveReader *reader, UmiKanbanCard *card)
{
    READ_TEXT(reader, card, id); READ_TEXT(reader, card, title);
    READ_TEXT(reader, card, description); READ_TEXT(reader, card, assignee);
    card->priority = (UmiKanbanPriority)UmiArchiveReadUnsigned(reader, UMI_KANBAN_URGENT);
    card->due_unix_seconds = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    READ_TEXT(reader, card, blocked_reason);
    card->label_count = (size_t)UmiArchiveReadUnsigned(reader, UMI_KANBAN_MAX_LABELS);
    for (size_t i = 0U; i < card->label_count; ++i)
        UmiArchiveReadText(reader, card->labels[i], sizeof(card->labels[i]));
    card->check_count = (size_t)UmiArchiveReadUnsigned(reader, UMI_KANBAN_MAX_CHECKS);
    for (size_t i = 0U; i < card->check_count; ++i) {
        UmiArchiveReadText(reader, card->checks[i].text, sizeof(card->checks[i].text));
        card->checks[i].complete = (int)UmiArchiveReadUnsigned(reader, 1U);
    }
    card->dependency_count = (size_t)UmiArchiveReadUnsigned(reader, UMI_KANBAN_MAX_DEPENDENCIES);
    for (size_t i = 0U; i < card->dependency_count; ++i)
        UmiArchiveReadText(reader, card->dependencies[i], sizeof(card->dependencies[i]));
}
/* The writer below appends an explicitly versioned developer directory before card rows; original schemas retain their exact field layout. Retained for engineering review. */
#if 0
static void Write(UmiArchiveWriter *writer, const UmiKanbanBoard *board)
{
    const UmiKanbanConfig *config = &board->config;
    WRITE_TEXT(writer, config, id); WRITE_TEXT(writer, config, title);
    UmiArchiveWriteUnsigned(writer, board->revision);
    UmiArchiveWriteUnsigned(writer, config->card_capacity);
    UmiArchiveWriteUnsigned(writer, config->column_count);
    for (size_t i = 0U; i < config->column_count; ++i) {
        const UmiKanbanColumn *column = &config->columns[i];
        WRITE_TEXT(writer, column, id); WRITE_TEXT(writer, column, title);
        UmiArchiveWriteUnsigned(writer, (uint64_t)column->stage);
        UmiArchiveWriteUnsigned(writer, column->wip_limit);
        UmiArchiveWriteUnsigned(writer, config->transitions[i]);
    }
    UmiArchiveWriteUnsigned(writer, board->count);
    for (size_t i = 0U; i < board->count; ++i) {
        UmiArchiveWriteUnsigned(writer, board->rows[i].column);
        UmiArchiveWriteUnsigned(writer, (uint64_t)board->rows[i].archived);
        WriteCard(writer, &board->rows[i].card);
    }
}
#endif

static void Write(UmiArchiveWriter *writer, const UmiKanbanBoard *board)
{
    const UmiKanbanConfig *config = &board->config;
    WRITE_TEXT(writer, config, id); WRITE_TEXT(writer, config, title);
    UmiArchiveWriteUnsigned(writer, board->revision);
    UmiArchiveWriteUnsigned(writer, config->card_capacity);
    UmiArchiveWriteUnsigned(writer, config->column_count);
    for (size_t i = 0U; i < config->column_count; ++i) {
        const UmiKanbanColumn *column = &config->columns[i];
        WRITE_TEXT(writer, column, id); WRITE_TEXT(writer, column, title);
        UmiArchiveWriteUnsigned(writer, (uint64_t)column->stage);
        UmiArchiveWriteUnsigned(writer, column->wip_limit);
        UmiArchiveWriteUnsigned(writer, config->transitions[i]);
    }
    if (board->developer_count != 0U) {
        UmiArchiveWriteUnsigned(writer, board->developer_count);
        for (size_t i = 0U; i < board->developer_count; ++i) {
            WRITE_TEXT(writer, &board->developers[i], id);
            WRITE_TEXT(writer, &board->developers[i], name);
        }
    }
    UmiArchiveWriteUnsigned(writer, board->count);
    for (size_t i = 0U; i < board->count; ++i) {
        UmiArchiveWriteUnsigned(writer, board->rows[i].column);
        UmiArchiveWriteUnsigned(writer, (uint64_t)board->rows[i].archived);
        WriteCard(writer, &board->rows[i].card);
    }
}
/* The encoder below selects the extended schema only for boards with Blocked columns; the original archive layout remains reviewable. Retained for engineering review. */
#if 0
UmiStatus umi_kanban_board_encode(const UmiKanbanBoard *board, uint64_t expected_revision,
    void *bytes, size_t capacity, size_t *out_size)
{
    if (board == NULL || out_size == NULL || (bytes == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (board->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE;
    UmiStatus status = UmiKanbanValidate(board, NULL);
    if (status != UMI_STATUS_OK) return status;
    UmiArchiveWriter measure = {NULL, UMI_VALUE_ARCHIVE_BYTE_LIMIT, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&measure, board);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    *out_size = measure.offset;
    if (bytes == NULL) return UMI_STATUS_OK;
    if (capacity < measure.offset) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiArchiveWriter writer = {bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&writer, board);
    UmiArchiveFinish(writer.bytes, writer.offset, KANBAN_SCHEMA);
    return writer.status;
}
#endif

/* The encoder below identifies persisted developer names with their own schema while preserving legacy board compatibility. Retained for engineering review. */
#if 0
UmiStatus umi_kanban_board_encode(const UmiKanbanBoard *board, uint64_t expected_revision,
    void *bytes, size_t capacity, size_t *out_size)
{
    if (board == NULL || out_size == NULL || (bytes == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (board->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE;
    UmiStatus status = UmiKanbanValidate(board, NULL);
    if (status != UMI_STATUS_OK) return status;
    UmiArchiveWriter measure = {NULL, UMI_VALUE_ARCHIVE_BYTE_LIMIT, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&measure, board);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    *out_size = measure.offset;
    if (bytes == NULL) return UMI_STATUS_OK;
    if (capacity < measure.offset) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiArchiveWriter writer = {bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&writer, board);
    uint64_t schema = KANBAN_SCHEMA;
    for (size_t i = 0U; i < board->config.column_count; ++i)
        if (board->config.columns[i].stage == UMI_KANBAN_BLOCKED) schema = KANBAN_WORKFLOW_SCHEMA;
    UmiArchiveFinish(writer.bytes, writer.offset, schema);
    return writer.status;
}
#endif

UmiStatus umi_kanban_board_encode(const UmiKanbanBoard *board, uint64_t expected_revision,
    void *bytes, size_t capacity, size_t *out_size)
{
    if (board == NULL || out_size == NULL || (bytes == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (board->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE;
    UmiStatus status = UmiKanbanValidate(board, NULL);
    if (status != UMI_STATUS_OK) return status;
    UmiArchiveWriter measure = {NULL, UMI_VALUE_ARCHIVE_BYTE_LIMIT, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&measure, board);
    if (measure.status != UMI_STATUS_OK) return measure.status;
    *out_size = measure.offset;
    if (bytes == NULL) return UMI_STATUS_OK;
    if (capacity < measure.offset) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiArchiveWriter writer = {bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    Write(&writer, board);
    uint64_t schema = KANBAN_SCHEMA;
    for (size_t i = 0U; i < board->config.column_count; ++i)
        if (board->config.columns[i].stage == UMI_KANBAN_BLOCKED) schema = KANBAN_WORKFLOW_SCHEMA;
    if (board->developer_count != 0U) schema = KANBAN_DEVELOPERS_SCHEMA;
    UmiArchiveFinish(writer.bytes, writer.offset, schema);
    return writer.status;
}
/* The decoder below reads both documented schemas and continues rejecting extended stages in a legacy envelope. Retained for engineering review. */
#if 0
UmiStatus umi_kanban_board_decode(const void *bytes, size_t size,
    UmiKanbanBoard **out_board, uint64_t *out_saved_revision)
{
    if (bytes == NULL || out_board == NULL || out_saved_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiValueArchiveInfo info;
    UmiStatus status = umi_value_archive_inspect(bytes, size, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.schema != KANBAN_SCHEMA) return UMI_STATUS_UNAVAILABLE;
    UmiKanbanBoard *board = calloc(1U, sizeof(*board));
    if (board == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    UmiKanbanConfig *config = &board->config;
    READ_TEXT(&reader, config, id); READ_TEXT(&reader, config, title);
    uint64_t saved_revision = UmiArchiveReadUnsigned(&reader, UINT64_MAX);
    config->card_capacity = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
    config->column_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_COLUMNS);
    for (size_t i = 0U; i < config->column_count; ++i) {
        UmiKanbanColumn *column = &config->columns[i];
        READ_TEXT(&reader, column, id); READ_TEXT(&reader, column, title);
        column->stage = (UmiKanbanStage)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_DONE);
        column->wip_limit = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
        config->transitions[i] = (uint32_t)UmiArchiveReadUnsigned(&reader, UINT32_MAX);
    }
    if (reader.status != UMI_STATUS_OK || saved_revision == 0U || UmiKanbanConfigCheck(config) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    board->count = (size_t)UmiArchiveReadUnsigned(&reader, config->card_capacity);
    if (reader.status != UMI_STATUS_OK) { status = reader.status; goto failed; }
    board->rows = calloc(config->card_capacity, sizeof(*board->rows));
    if (board->rows == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto failed; }
    for (size_t i = 0U; i < board->count; ++i) {
        board->rows[i].column = (size_t)UmiArchiveReadUnsigned(&reader, config->column_count - 1U);
        board->rows[i].archived = (int)UmiArchiveReadUnsigned(&reader, 1U);
        ReadCard(&reader, &board->rows[i].card);
    }
    if (reader.status != UMI_STATUS_OK || reader.offset != size || UmiKanbanValidate(board, NULL) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    /* Imported revision and old activities do not become publication authority. */
    board->revision = 1U; board->next_sequence = 1U;
    *out_board = board; *out_saved_revision = saved_revision;
    return UMI_STATUS_OK;
failed:
    umi_kanban_board_destroy(board);
    return status;
}
#endif

/* The reader below accepts the developer-directory schema and validates every identity before exposing a board. Earlier readers remain retained for review. Retained for engineering review. */
#if 0
UmiStatus umi_kanban_board_decode(const void *bytes, size_t size,
    UmiKanbanBoard **out_board, uint64_t *out_saved_revision)
{
    if (bytes == NULL || out_board == NULL || out_saved_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiValueArchiveInfo info;
    UmiStatus status = umi_value_archive_inspect(bytes, size, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.schema != KANBAN_SCHEMA && info.schema != KANBAN_WORKFLOW_SCHEMA) return UMI_STATUS_UNAVAILABLE;
    UmiKanbanBoard *board = calloc(1U, sizeof(*board));
    if (board == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    UmiKanbanConfig *config = &board->config;
    READ_TEXT(&reader, config, id); READ_TEXT(&reader, config, title);
    uint64_t saved_revision = UmiArchiveReadUnsigned(&reader, UINT64_MAX);
    config->card_capacity = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
    config->column_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_COLUMNS);
    for (size_t i = 0U; i < config->column_count; ++i) {
        UmiKanbanColumn *column = &config->columns[i];
        READ_TEXT(&reader, column, id); READ_TEXT(&reader, column, title);
        column->stage = (UmiKanbanStage)UmiArchiveReadUnsigned(&reader,
            info.schema == KANBAN_SCHEMA ? UMI_KANBAN_DONE : UMI_KANBAN_BLOCKED);
        column->wip_limit = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
        config->transitions[i] = (uint32_t)UmiArchiveReadUnsigned(&reader, UINT32_MAX);
    }
    if (reader.status != UMI_STATUS_OK || saved_revision == 0U || UmiKanbanConfigCheck(config) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    board->count = (size_t)UmiArchiveReadUnsigned(&reader, config->card_capacity);
    if (reader.status != UMI_STATUS_OK) { status = reader.status; goto failed; }
    board->rows = calloc(config->card_capacity, sizeof(*board->rows));
    if (board->rows == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto failed; }
    for (size_t i = 0U; i < board->count; ++i) {
        board->rows[i].column = (size_t)UmiArchiveReadUnsigned(&reader, config->column_count - 1U);
        board->rows[i].archived = (int)UmiArchiveReadUnsigned(&reader, 1U);
        ReadCard(&reader, &board->rows[i].card);
    }
    if (reader.status != UMI_STATUS_OK || reader.offset != size || UmiKanbanValidate(board, NULL) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    /* Imported revision and old activities do not become publication authority. */
    board->revision = 1U; board->next_sequence = 1U;
    *out_board = board; *out_saved_revision = saved_revision;
    return UMI_STATUS_OK;
failed:
    umi_kanban_board_destroy(board);
    return status;
}
#endif

UmiStatus umi_kanban_board_decode(const void *bytes, size_t size,
    UmiKanbanBoard **out_board, uint64_t *out_saved_revision)
{
    if (bytes == NULL || out_board == NULL || out_saved_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiValueArchiveInfo info;
    UmiStatus status = umi_value_archive_inspect(bytes, size, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.schema != KANBAN_SCHEMA && info.schema != KANBAN_WORKFLOW_SCHEMA &&
        info.schema != KANBAN_DEVELOPERS_SCHEMA) return UMI_STATUS_UNAVAILABLE;
    UmiKanbanBoard *board = calloc(1U, sizeof(*board));
    if (board == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
    UmiKanbanConfig *config = &board->config;
    READ_TEXT(&reader, config, id); READ_TEXT(&reader, config, title);
    uint64_t saved_revision = UmiArchiveReadUnsigned(&reader, UINT64_MAX);
    config->card_capacity = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
    config->column_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_COLUMNS);
    for (size_t i = 0U; i < config->column_count; ++i) {
        UmiKanbanColumn *column = &config->columns[i];
        READ_TEXT(&reader, column, id); READ_TEXT(&reader, column, title);
        column->stage = (UmiKanbanStage)UmiArchiveReadUnsigned(&reader,
            info.schema == KANBAN_SCHEMA ? UMI_KANBAN_DONE : UMI_KANBAN_BLOCKED);
        column->wip_limit = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_CARDS);
        config->transitions[i] = (uint32_t)UmiArchiveReadUnsigned(&reader, UINT32_MAX);
    }
    if (reader.status != UMI_STATUS_OK || saved_revision == 0U || UmiKanbanConfigCheck(config) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    if (info.schema == KANBAN_DEVELOPERS_SCHEMA) {
        board->developer_count = (size_t)UmiArchiveReadUnsigned(&reader, UMI_KANBAN_MAX_DEVELOPERS);
        for (size_t i = 0U; i < board->developer_count; ++i) {
            READ_TEXT(&reader, &board->developers[i], id);
            READ_TEXT(&reader, &board->developers[i], name);
        }
        if (reader.status != UMI_STATUS_OK || UmiKanbanDevelopersCheck(board) != UMI_STATUS_OK) {
            status = UMI_STATUS_PARSE_ERROR; goto failed;
        }
    }
    board->count = (size_t)UmiArchiveReadUnsigned(&reader, config->card_capacity);
    if (reader.status != UMI_STATUS_OK) { status = reader.status; goto failed; }
    board->rows = calloc(config->card_capacity, sizeof(*board->rows));
    if (board->rows == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto failed; }
    for (size_t i = 0U; i < board->count; ++i) {
        board->rows[i].column = (size_t)UmiArchiveReadUnsigned(&reader, config->column_count - 1U);
        board->rows[i].archived = (int)UmiArchiveReadUnsigned(&reader, 1U);
        ReadCard(&reader, &board->rows[i].card);
    }
    if (reader.status != UMI_STATUS_OK || reader.offset != size || UmiKanbanValidate(board, NULL) != UMI_STATUS_OK) {
        status = UMI_STATUS_PARSE_ERROR; goto failed;
    }
    /* Imported revision and old activities do not become publication authority. */
    board->revision = 1U; board->next_sequence = 1U;
    *out_board = board; *out_saved_revision = saved_revision;
    return UMI_STATUS_OK;
failed:
    umi_kanban_board_destroy(board);
    return status;
}
#undef READ_TEXT
#undef WRITE_TEXT
