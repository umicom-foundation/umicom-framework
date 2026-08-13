#include "umicom/diagnostic_ui/projection.h"

#include <stdio.h>
#include <string.h>

static int diagnostic_ui_severity(UmiDiagnosticSeverity severity)
{
    if (severity >= UMI_DIAGNOSTIC_ERROR) return 4;
    if (severity == UMI_DIAGNOSTIC_WARNING) return 3;
    return 1;
}

UmiStatus umi_diagnostic_ui_project_problem(const UmiDiagnosticSnapshot *diagnostic,
                                            UmiUiProblemSnapshot *out_problem)
{
    if (diagnostic == NULL || out_problem == NULL ||
        umi_diagnostic_snapshot_validate(diagnostic, NULL, 0U) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_problem, 0, sizeof(*out_problem));
    out_problem->struct_size = (uint32_t)sizeof(*out_problem);
    out_problem->api_version = UMI_UI_PROBLEM_API_VERSION;
    (void)snprintf(out_problem->id, sizeof(out_problem->id), "%s", diagnostic->id);
    (void)snprintf(out_problem->source, sizeof(out_problem->source), "%s", diagnostic->source);
    (void)snprintf(out_problem->code, sizeof(out_problem->code), "%s", diagnostic->code);
    (void)snprintf(out_problem->message, sizeof(out_problem->message), "%s", diagnostic->message);
    (void)snprintf(out_problem->uri, sizeof(out_problem->uri), "%s", diagnostic->uri);
    out_problem->line = diagnostic->line;
    out_problem->column = diagnostic->column;
    out_problem->severity = diagnostic_ui_severity(diagnostic->severity);
    out_problem->resolved = diagnostic->resolved;
    out_problem->revision = diagnostic->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_ui_project_output_channel(const UmiOutputBuffer *buffer,
                                                   const char *channel_id,
                                                   UmiUiOutputChannelSnapshot *out_channel)
{
    size_t count;
    size_t index;
    size_t used = 0U;
    int found = 0;
    if (buffer == NULL || channel_id == NULL || channel_id[0] == '\0' || out_channel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_channel, 0, sizeof(*out_channel));
    out_channel->struct_size = (uint32_t)sizeof(*out_channel);
    out_channel->api_version = UMI_UI_OUTPUT_CHANNEL_API_VERSION;
    (void)snprintf(out_channel->id, sizeof(out_channel->id), "%s", channel_id);
    count = umi_output_buffer_count(buffer);
    for (index = 0U; index < count; ++index) {
        UmiOutputRecord record;
        size_t available;
        size_t length;
        if (umi_output_buffer_at(buffer, index, &record) != UMI_STATUS_OK ||
            strcmp(record.channel_id, channel_id) != 0) continue;
        found = 1;
        (void)snprintf(out_channel->name, sizeof(out_channel->name), "%s",
                       record.channel_name[0] != '\0' ? record.channel_name : record.channel_id);
        (void)snprintf(out_channel->category, sizeof(out_channel->category), "%s", record.source);
        out_channel->sequence = record.sequence;
        out_channel->revision = record.revision;
        available = sizeof(out_channel->text) - used;
        length = strlen(record.text);
        if (available > 1U) {
            size_t copied = length < available - 1U ? length : available - 1U;
            (void)memcpy(out_channel->text + used, record.text, copied);
            used += copied;
            if (used + 1U < sizeof(out_channel->text) &&
                (used == 0U || out_channel->text[used - 1U] != '\n')) {
                out_channel->text[used++] = '\n';
            }
            out_channel->text[used] = '\0';
        }
    }
    out_channel->visible = 1;
    out_channel->preserve = 1;
    return found != 0 ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_diagnostic_ui_sync_registries(UmiDiagnosticPipeline *pipeline,
                                            UmiUiProblemRegistry *problems,
                                            UmiUiOutputChannelRegistry *channels)
{
    UmiDiagnosticModel *model;
    UmiOutputBuffer *output;
    size_t count;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (pipeline == NULL || problems == NULL || channels == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model = umi_diagnostic_pipeline_model(pipeline);
    output = umi_diagnostic_pipeline_output(pipeline);
    umi_ui_problem_registry_clear(problems);
    umi_ui_output_channel_registry_clear(channels);
    count = umi_diagnostic_model_count(model);
    for (index = 0U; index < count && status == UMI_STATUS_OK; ++index) {
        UmiDiagnosticSnapshot diagnostic;
        UmiUiProblemSnapshot problem;
        status = umi_diagnostic_model_at(model, index, &diagnostic);
        if (status == UMI_STATUS_OK) status = umi_diagnostic_ui_project_problem(&diagnostic, &problem);
        if (status == UMI_STATUS_OK) status = umi_ui_problem_registry_upsert(problems, &problem);
    }
    count = umi_output_buffer_count(output);
    for (index = 0U; index < count && status == UMI_STATUS_OK; ++index) {
        UmiOutputRecord record;
        UmiUiOutputChannelSnapshot channel;
        size_t prior;
        int first = 1;
        status = umi_output_buffer_at(output, index, &record);
        for (prior = 0U; prior < index && status == UMI_STATUS_OK; ++prior) {
            UmiOutputRecord previous;
            status = umi_output_buffer_at(output, prior, &previous);
            if (status == UMI_STATUS_OK && strcmp(previous.channel_id, record.channel_id) == 0) first = 0;
        }
        if (status == UMI_STATUS_OK && first != 0) {
            status = umi_diagnostic_ui_project_output_channel(output, record.channel_id, &channel);
            if (status == UMI_STATUS_OK) status = umi_ui_output_channel_registry_upsert(channels, &channel);
        }
    }
    return status;
}
