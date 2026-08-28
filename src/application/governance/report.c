/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/governance/report.c
 *
 * PURPOSE:
 *   Serialize component-governance inventory data with bounded writes and a
 *   two-pass sizing contract suitable for native tools and application UIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/governance/report.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct ReportWriter {
  char *output;
  size_t capacity;
  size_t required;
} ReportWriter;

static void writer_append(ReportWriter *writer, const char *format, ...) {
  va_list arguments;
  va_list sizing_arguments;
  int length;

  va_start(arguments, format);
  va_copy(sizing_arguments, arguments);
  length = vsnprintf(NULL, 0U, format, sizing_arguments);
  va_end(sizing_arguments);
  if (length < 0) {
    va_end(arguments);
    return;
  }
  if (writer->output != NULL && writer->required < writer->capacity) {
    (void)vsnprintf(writer->output + writer->required, writer->capacity - writer->required, format,
                    arguments);
  }
  writer->required += (size_t)length;
  va_end(arguments);
}

static void writer_json_string(ReportWriter *writer, const char *value) {
  const unsigned char *cursor = (const unsigned char *)(value != NULL ? value : "");

  writer_append(writer, "\"");
  while (*cursor != 0U) {
    if (*cursor == '"' || *cursor == '\\') {
      writer_append(writer, "\\%c", *cursor);
    } else if (*cursor == '\n') {
      writer_append(writer, "\\n");
    } else if (*cursor == '\r') {
      writer_append(writer, "\\r");
    } else if (*cursor == '\t') {
      writer_append(writer, "\\t");
    } else if (*cursor < 0x20U) {
      writer_append(writer, "\\u%04x", (unsigned)*cursor);
    } else {
      writer_append(writer, "%c", *cursor);
    }
    cursor += 1U;
  }
  writer_append(writer, "\"");
}

static void write_text_record(ReportWriter *writer, const UmiComponentGovernanceRecord *record) {
  writer_append(writer, "%s|%s|%s|%s|%s|0x%02x|0x%02x|0x%02x\n", record->definition->component_id,
                record->definition->domain_id,
                umi_application_component_role_text(record->definition->role),
                umi_component_api_status_text(record->api_status),
                umi_component_owner_text(record->owner), (unsigned)record->required_evidence,
                (unsigned)record->available_evidence, (unsigned)record->frontend_support);
}

static void write_markdown_record(ReportWriter *writer,
                                  const UmiComponentGovernanceRecord *record) {
  writer_append(
      writer, "| `%s` | %s | %s | %s | %s | %s |\n", record->definition->component_id,
      record->definition->domain_id, umi_application_component_role_text(record->definition->role),
      umi_component_api_status_text(record->api_status), umi_component_owner_text(record->owner),
      umi_component_evidence_is_complete(record) ? "complete" : "incomplete");
}

static void write_json_record(ReportWriter *writer, const UmiComponentGovernanceRecord *record) {
  writer_append(writer, "    {\"component_id\":");
  writer_json_string(writer, record->definition->component_id);
  writer_append(writer, ",\"domain_id\":");
  writer_json_string(writer, record->definition->domain_id);
  writer_append(writer, ",\"title\":");
  writer_json_string(writer, record->definition->title);
  writer_append(writer, ",\"capability_id\":");
  writer_json_string(writer, record->definition->capability_id);
  writer_append(writer, ",\"role\":");
  writer_json_string(writer, umi_application_component_role_text(record->definition->role));
  writer_append(writer, ",\"api_status\":");
  writer_json_string(writer, umi_component_api_status_text(record->api_status));
  writer_append(writer, ",\"owner\":");
  writer_json_string(writer, umi_component_owner_text(record->owner));
  writer_append(writer,
                ",\"required_evidence\":%u,\"available_evidence\":%u,"
                "\"frontend_support\":%u,\"evidence_complete\":%s,"
                "\"introduced_version\":",
                (unsigned)record->required_evidence, (unsigned)record->available_evidence,
                (unsigned)record->frontend_support,
                umi_component_evidence_is_complete(record) ? "true" : "false");
  writer_json_string(writer, record->introduced_version);
  writer_append(writer, ",\"replacement_component_id\":");
  if (record->replacement_component_id != NULL) {
    writer_json_string(writer, record->replacement_component_id);
  } else {
    writer_append(writer, "null");
  }
  writer_append(writer, "}");
}

static void write_report(ReportWriter *writer, const UmiComponentInventory *inventory,
                         const UmiComponentQueryResult *result, UmiComponentReportFormat format) {
  size_t index;

  if (format == UMI_COMPONENT_REPORT_TEXT) {
    writer_append(writer,
                  "components=%zu domains=%zu evidence_complete=%zu "
                  "evidence_incomplete=%zu selected=%zu\n",
                  inventory->component_count, inventory->domain_count,
                  inventory->evidence_complete_count, inventory->evidence_incomplete_count,
                  result->count);
    writer_append(writer, "component_id|domain|role|status|owner|required|available|frontends\n");
  } else if (format == UMI_COMPONENT_REPORT_MARKDOWN) {
    writer_append(writer, "# Umicom Framework Component Inventory\n\n");
    writer_append(writer,
                  "Components: **%zu** · Domains: **%zu** · Evidence complete: "
                  "**%zu** · Evidence incomplete: **%zu** · Selected: **%zu**\n\n",
                  inventory->component_count, inventory->domain_count,
                  inventory->evidence_complete_count, inventory->evidence_incomplete_count,
                  result->count);
    writer_append(writer, "| Component | Domain | Role | Status | Owner | Evidence |\n"
                          "|---|---|---|---|---|---|\n");
  } else {
    writer_append(writer,
                  "{\n  \"summary\":{\"component_count\":%zu,"
                  "\"domain_count\":%zu,\"evidence_complete_count\":%zu,"
                  "\"evidence_incomplete_count\":%zu,\"selected_count\":%zu},"
                  "\n  \"components\":[\n",
                  inventory->component_count, inventory->domain_count,
                  inventory->evidence_complete_count, inventory->evidence_incomplete_count,
                  result->count);
  }

  for (index = 0U; index < result->count; ++index) {
    const UmiComponentGovernanceRecord *record =
        umi_component_query_result_at(inventory, result, index);
    if (format == UMI_COMPONENT_REPORT_TEXT) {
      write_text_record(writer, record);
    } else if (format == UMI_COMPONENT_REPORT_MARKDOWN) {
      write_markdown_record(writer, record);
    } else {
      write_json_record(writer, record);
      writer_append(writer, index + 1U < result->count ? ",\n" : "\n");
    }
  }
  if (format == UMI_COMPONENT_REPORT_JSON) {
    writer_append(writer, "  ]\n}\n");
  }
}

UmiStatus umi_component_inventory_report_write(const UmiComponentInventory *inventory,
                                               const UmiComponentQuery *query,
                                               UmiComponentReportFormat format, char *output,
                                               size_t output_capacity,
                                               size_t *out_required_capacity) {
  UmiComponentQuery all_query;
  UmiComponentQueryResult result;
  ReportWriter writer;
  UmiStatus status;

  if (inventory == NULL || out_required_capacity == NULL || format < UMI_COMPONENT_REPORT_TEXT ||
      format > UMI_COMPONENT_REPORT_JSON || (output == NULL && output_capacity != 0U)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  if (query == NULL) {
    umi_component_query_init(&all_query);
    query = &all_query;
  }
  status = umi_component_inventory_query(inventory, query, &result);
  if (status != UMI_STATUS_OK)
    return status;

  writer.output = output;
  writer.capacity = output_capacity;
  writer.required = 0U;
  if (output != NULL && output_capacity > 0U)
    output[0] = '\0';
  write_report(&writer, inventory, &result, format);
  *out_required_capacity = writer.required + 1U;
  if (output != NULL && output_capacity > 0U) {
    output[output_capacity - 1U] = '\0';
  }
  return output == NULL || output_capacity >= writer.required + 1U ? UMI_STATUS_OK
                                                                   : UMI_STATUS_CAPACITY_EXCEEDED;
}
