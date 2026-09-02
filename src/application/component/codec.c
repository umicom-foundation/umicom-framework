/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/codec.c
 *
 * PURPOSE:
 *   Encode and decode the versioned, line-oriented Umicom workspace format.
 *   Reserved text characters use percent escapes so files remain readable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/codec.h"

#include <limits.h>
#include <string.h>

#include "umicom/application/experience/catalogue.h"
#include "umicom/base/text.h"

typedef struct WorkspaceWriter {
  char *buffer;
  size_t capacity;
  size_t length;
  UmiStatus status;
} WorkspaceWriter;

/* Provide the writer character operation used by this module and its client applications. */
static void writer_character(WorkspaceWriter *writer, char value) {
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (writer->status != UMI_STATUS_OK)
    return;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (writer->length + 1U >= writer->capacity) {
    writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  writer->buffer[writer->length++] = value;
  writer->buffer[writer->length] = '\0';
}

/* Provide the writer text operation used by this module and its client applications. */
static void writer_text(WorkspaceWriter *writer, const char *value) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL) {
    writer->status = UMI_STATUS_INVALID_ARGUMENT;
    return;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; value[index] != '\0'; ++index)
    writer_character(writer, value[index]);
}

/* Provide the hexadecimal operation used by this module and its client applications. */
static char hexadecimal(unsigned int value) {
  return (char)(value < 10U ? '0' + value : 'A' + (value - 10U));
}

/* Provide the writer encoded operation used by this module and its client applications. */
static void writer_encoded(WorkspaceWriter *writer, const char *value) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL) {
    writer->status = UMI_STATUS_INVALID_ARGUMENT;
    return;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; value[index] != '\0'; ++index) {
    unsigned int byte = (unsigned char)value[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte == (unsigned int)'%' || byte == (unsigned int)'|' || byte == (unsigned int)'\r' ||
        byte == (unsigned int)'\n') {
      writer_character(writer, '%');
      writer_character(writer, hexadecimal((byte >> 4U) & 0xFU));
      writer_character(writer, hexadecimal(byte & 0xFU));
    } /* Use this fallback path when the earlier condition does not apply. */ else {
      writer_character(writer, (char)byte);
    }
  }
}

/* Provide the writer number operation used by this module and its client applications. */
static void writer_number(WorkspaceWriter *writer, uint64_t value) {
  char text[32];
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (writer->status == UMI_STATUS_OK)
    writer->status = umi_text_format(text, sizeof(text), "%llu", (unsigned long long)value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (writer->status == UMI_STATUS_OK)
    writer_text(writer, text);
}

/* Provide the writer property operation used by this module and its client applications. */
static void writer_property(WorkspaceWriter *writer, const char *name, const char *value) {
  writer_text(writer, name);
  writer_character(writer, '|');
  writer_encoded(writer, value);
  writer_character(writer, '\n');
}

/*
 * Write application component workspace in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus
umi_application_component_workspace_encode(const UmiApplicationComponentWorkspaceDraft *draft,
                                           char *buffer, size_t capacity, size_t *out_length) {
  WorkspaceWriter writer;
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (draft == NULL || buffer == NULL || capacity == 0U || out_length == NULL ||
      draft->recipe_id[0] == '\0' || draft->application_id[0] == '\0' || draft->title[0] == '\0' ||
      draft->description[0] == '\0' || draft->experience_profile_id[0] == '\0' ||
      draft->slot_count == 0U || draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  writer.buffer = buffer;
  writer.capacity = capacity;
  writer.length = 0U;
  writer.status = UMI_STATUS_OK;
  buffer[0] = '\0';
  writer_text(&writer, "umicom-workspace|");
  writer_number(&writer, UMI_APPLICATION_COMPONENT_WORKSPACE_FORMAT_VERSION);
  writer_character(&writer, '\n');
  writer_property(&writer, "recipe", draft->recipe_id);
  writer_property(&writer, "application", draft->application_id);
  writer_property(&writer, "experience", draft->experience_profile_id);
  writer_property(&writer, "title", draft->title);
  writer_property(&writer, "description", draft->description);
  writer_text(&writer, "audience|");
  writer_number(&writer, (uint64_t)draft->audience);
  writer_character(&writer, '\n');
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < draft->slot_count; ++index) {
    const UmiApplicationComponentDraftSlot *slot = &draft->slots[index];
    writer_text(&writer, "slot|");
    writer_encoded(&writer, slot->component_id);
    writer_character(&writer, '|');
    writer_encoded(&writer, slot->instance_id);
    writer_character(&writer, '|');
    writer_number(&writer, (uint64_t)slot->region);
    writer_character(&writer, '|');
    writer_number(&writer, (uint64_t)slot->weight);
    writer_character(&writer, '|');
    writer_number(&writer, (uint64_t)(slot->visible != 0));
    writer_character(&writer, '|');
    writer_number(&writer, (uint64_t)(slot->locked != 0));
    writer_character(&writer, '\n');
  }
  writer_text(&writer, "end\n");
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (writer.status != UMI_STATUS_OK) {
    buffer[0] = '\0';
    *out_length = 0U;
    return writer.status;
  }
  *out_length = writer.length;
  return UMI_STATUS_OK;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char value) {
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (value >= '0' && value <= '9')
    return value - '0';
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (value >= 'A' && value <= 'F')
    return value - 'A' + 10;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (value >= 'a' && value <= 'f')
    return value - 'a' + 10;
  return -1;
}

/* Provide the read field operation used by this module and its client applications. */
static UmiStatus read_field(const char *line, size_t line_length, size_t *cursor, char *out_value,
                            size_t capacity) {
  size_t written = 0U;
  size_t position;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (line == NULL || cursor == NULL || out_value == NULL || capacity == 0U ||
      *cursor > line_length)
    return UMI_STATUS_INVALID_ARGUMENT;
  position = *cursor;
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (position < line_length && line[position] != '|') {
    unsigned int byte = (unsigned char)line[position++];
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte == (unsigned int)'%') {
      int high;
      int low;
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (position + 1U >= line_length)
        return UMI_STATUS_INVALID_ARGUMENT;
      high = hex_value(line[position]);
      low = hex_value(line[position + 1U]);
      /* Apply this branch only when its contract condition is satisfied. */
      if (high < 0 || low < 0 || (high == 0 && low == 0))
        return UMI_STATUS_INVALID_ARGUMENT;
      byte = (unsigned int)((high << 4) | low);
      position += 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written + 1U >= capacity)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    out_value[written++] = (char)byte;
  }
  out_value[written] = '\0';
  *cursor = position < line_length ? position + 1U : line_length + 1U;
  return UMI_STATUS_OK;
}

/* Provide the parse number operation used by this module and its client applications. */
static UmiStatus parse_number(const char *text, uint32_t *out_value) {
  uint64_t value = 0U;
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (text == NULL || out_value == NULL || text[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; text[index] != '\0'; ++index) {
    unsigned int digit;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (text[index] < '0' || text[index] > '9')
      return UMI_STATUS_INVALID_ARGUMENT;
    digit = (unsigned int)(text[index] - '0');
    /* Apply this branch only when its contract condition is satisfied. */
    if (value > (UINT_MAX - digit) / 10U)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    value = value * 10U + digit;
  }
  *out_value = (uint32_t)value;
  return UMI_STATUS_OK;
}

/*
 * Provide the read text property operation used by this module and its client
 * applications.
 */
static UmiStatus read_text_property(const char *line, size_t line_length, size_t cursor,
                                    char *destination, size_t capacity) {
  UmiStatus status = read_field(line, line_length, &cursor, destination, capacity);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && cursor != line_length + 1U)
    return UMI_STATUS_INVALID_ARGUMENT;
  return status;
}

/* Provide the read slot operation used by this module and its client applications. */
static UmiStatus read_slot(const char *line, size_t line_length, size_t cursor,
                           UmiApplicationComponentDraftSlot *slot, size_t order) {
  char number[32];
  uint32_t value;
  UmiStatus status;
  (void)memset(slot, 0, sizeof(*slot));
  status = read_field(line, line_length, &cursor, slot->component_id, sizeof(slot->component_id));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = read_field(line, line_length, &cursor, slot->instance_id, sizeof(slot->instance_id));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = read_field(line, line_length, &cursor, number, sizeof(number));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = parse_number(number, &value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    slot->region = (UmiApplicationComponentRegion)value;
    status = read_field(line, line_length, &cursor, number, sizeof(number));
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = parse_number(number, &slot->weight);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = read_field(line, line_length, &cursor, number, sizeof(number));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = parse_number(number, &value);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (status == UMI_STATUS_OK && value <= 1U) {
    slot->visible = value != 0U;
    status = read_field(line, line_length, &cursor, number, sizeof(number));
  } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
    status = UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = parse_number(number, &value);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (status == UMI_STATUS_OK && value <= 1U)
    slot->locked = value != 0U;
  else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK)
    status = UMI_STATUS_INVALID_ARGUMENT;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && (cursor != line_length + 1U || slot->component_id[0] == '\0' ||
                                  slot->instance_id[0] == '\0' || slot->weight == 0U ||
                                  slot->region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
                                  slot->region > UMI_APPLICATION_COMPONENT_REGION_FLOATING))
    status = UMI_STATUS_INVALID_ARGUMENT;
  slot->order = (uint32_t)order;
  return status;
}

/* Provide the decode line operation used by this module and its client applications. */
static UmiStatus decode_line(const char *line, size_t line_length,
                             UmiApplicationComponentWorkspaceDraft *draft, unsigned int *metadata,
                             int *ended, uint32_t *format_version) {
  char keyword[32];
  char number[32];
  size_t cursor = 0U;
  uint32_t value;
  UmiStatus status = read_field(line, line_length, &cursor, keyword, sizeof(keyword));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "umicom-workspace") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 32U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 32U;
    status = read_field(line, line_length, &cursor, number, sizeof(number));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
      status = parse_number(number, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        (value == 1U || value == UMI_APPLICATION_COMPONENT_WORKSPACE_FORMAT_VERSION) &&
        cursor == line_length + 1U) {
      *format_version = value;
      return UMI_STATUS_OK;
    }
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "recipe") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 1U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 1U;
    return read_text_property(line, line_length, cursor, draft->recipe_id,
                              sizeof(draft->recipe_id));
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "application") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 2U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 2U;
    return read_text_property(line, line_length, cursor, draft->application_id,
                              sizeof(draft->application_id));
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "experience") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 64U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 64U;
    return read_text_property(line, line_length, cursor, draft->experience_profile_id,
                              sizeof(draft->experience_profile_id));
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "title") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 4U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 4U;
    return read_text_property(line, line_length, cursor, draft->title, sizeof(draft->title));
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "description") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 8U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 8U;
    return read_text_property(line, line_length, cursor, draft->description,
                              sizeof(draft->description));
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "audience") == 0) {
    /* Apply this branch only when its contract condition is satisfied. */
    if ((*metadata & 16U) != 0U)
      return UMI_STATUS_ALREADY_EXISTS;
    *metadata |= 16U;
    status = read_field(line, line_length, &cursor, number, sizeof(number));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
      status = parse_number(number, &value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK && value >= UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING &&
        value <= UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS && cursor == line_length + 1U)
      draft->audience = (UmiApplicationComponentRecipeAudience)value;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK)
      status = UMI_STATUS_INVALID_ARGUMENT;
    return status;
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "slot") == 0) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (draft->slot_count >= UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    status =
        read_slot(line, line_length, cursor, &draft->slots[draft->slot_count], draft->slot_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
      draft->slot_count += 1U;
    return status;
  }
  /* Use the stable identifier comparison to choose the matching record or policy. */
  if (strcmp(keyword, "end") == 0 && cursor == line_length + 1U) {
    *ended = 1;
    return UMI_STATUS_OK;
  }
  return UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Read application component workspace into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus
umi_application_component_workspace_decode(const char *text, size_t length,
                                           UmiApplicationComponentWorkspaceDraft *out_draft) {
  size_t line_start = 0U;
  size_t line_number = 0U;
  unsigned int metadata = 0U;
  uint32_t format_version = 0U;
  int ended = 0;
  UmiStatus status = UMI_STATUS_OK;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (text == NULL || length == 0U || out_draft == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(out_draft, 0, sizeof(*out_draft));
  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (status == UMI_STATUS_OK && line_start < length && !ended) {
    size_t line_end = line_start;
    size_t line_length;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (line_end < length && text[line_end] != '\n')
      line_end += 1U;
    line_length = line_end - line_start;
    /* Apply this branch only when its contract condition is satisfied. */
    if (line_length > 0U && text[line_start + line_length - 1U] == '\r')
      line_length -= 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (line_length > 0U) {
      status = decode_line(text + line_start, line_length, out_draft, &metadata, &ended,
                           &format_version);
      /* Apply this branch only when its contract condition is satisfied. */
      if (line_number == 0U &&
          (line_length < 17U || strncmp(text + line_start, "umicom-workspace|", 17U) != 0))
        status = UMI_STATUS_INVALID_ARGUMENT;
      line_number += 1U;
    }
    line_start = line_end < length ? line_end + 1U : length;
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    size_t remaining;
    /* Visit each bounded item once so every record receives the same rule. */
    for (remaining = line_start; remaining < length; ++remaining) {
      /* Apply this branch only when its contract condition is satisfied. */
      if (text[remaining] != '\r' && text[remaining] != '\n' && text[remaining] != ' ' &&
          text[remaining] != '\t') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
      }
    }
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK && format_version == 1U) {
    const UmiApplicationExperienceProfile *profile =
        umi_application_experience_profile_catalogue_for_recipe(out_draft->recipe_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL)
      status = UMI_STATUS_NOT_FOUND;
    /* Use this fallback path when the earlier condition does not apply. */
    else
      status = umi_text_copy(out_draft->experience_profile_id,
                             sizeof(out_draft->experience_profile_id), profile->profile_id);
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK &&
      (!ended || metadata != (format_version == 1U ? 63U : 127U) || out_draft->slot_count == 0U ||
       out_draft->recipe_id[0] == '\0' || out_draft->application_id[0] == '\0' ||
       out_draft->experience_profile_id[0] == '\0' || out_draft->title[0] == '\0' ||
       out_draft->description[0] == '\0'))
    status = UMI_STATUS_INVALID_ARGUMENT;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)memset(out_draft, 0, sizeof(*out_draft));
    return status;
  }
  out_draft->revision = 1U;
  out_draft->dirty = 0;
  return UMI_STATUS_OK;
}
