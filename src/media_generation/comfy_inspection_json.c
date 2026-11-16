/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_inspection_json.c
 *
 * PURPOSE:
 *   Render copied workflow findings through the shared JSON writer with staged output publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/protocol/json_writer.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void InspectionCount(UmiJsonWriter *writer, const char *key, size_t count)
{
    UmiJsonWriterRaw(writer, ",");
    UmiJsonWriterString(writer, key);
    UmiJsonWriterRaw(writer, ":");
    UmiJsonWriterUInt64(writer, (uint64_t)count);
}
static void InspectionDigest(UmiJsonWriter *writer, const char *key,
    const unsigned char digest[UMI_SHA256_BYTES])
{
    char text[65];
    UmiSha256Hex(digest, text);
    UmiJsonWriterRaw(writer, ",");
    UmiJsonWriterString(writer, key);
    UmiJsonWriterRaw(writer, ":");
    UmiJsonWriterString(writer, text);
}
static const char *InspectionKind(UmiComfyPreflightIssueKind kind)
{
    switch (kind) {
    case UMI_COMFY_PREFLIGHT_CLASS_MISSING: return "class-missing";
    case UMI_COMFY_PREFLIGHT_INPUT_MISSING: return "input-missing";
    case UMI_COMFY_PREFLIGHT_DEPRECATED: return "deprecated";
    case UMI_COMFY_PREFLIGHT_EXPERIMENTAL: return "experimental";
    case UMI_COMFY_PREFLIGHT_API_NODE: return "api-node";
    default: return NULL;
    }
}
UmiStatus UmiComfyInspectionJson(const UmiComfyPreflightReport *report,
    char *out, size_t capacity, size_t *out_length)
{
    if (out == NULL || out_length == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfyPreflightSummary summary;
    UmiStatus status = UmiComfyPreflightInspect(report, &summary);
    if (status != UMI_STATUS_OK) return status;
    size_t limit = capacity < UMI_COMFY_INSPECTION_JSON_CAPACITY ? capacity : UMI_COMFY_INSPECTION_JSON_CAPACITY;
    char *staged = malloc(limit);
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, staged, limit);
    UmiJsonWriterRaw(&writer, "{\"schema_version\":1,\"scope\":\"class-and-required-field-presence\",\"all_node_requirements_observed\":");
    UmiJsonWriterBool(&writer, summary.all_node_requirements_observed);
    char revision[32];
    int written = snprintf(revision, sizeof(revision), "%" PRIu64, summary.connection_revision);
    if (written < 0 || (size_t)written >= sizeof(revision)) { status = UMI_STATUS_INVALID_STATE; goto done; }
    UmiJsonWriterRaw(&writer, ",\"connection_revision\":");
    UmiJsonWriterString(&writer, revision);
    InspectionDigest(&writer, "connection_sha256", summary.connection_sha256);
    InspectionDigest(&writer, "request_sha256", summary.request_sha256);
    InspectionDigest(&writer, "catalogue_sha256", summary.catalogue_sha256);
    InspectionCount(&writer, "node_count", summary.node_count);
    InspectionCount(&writer, "known_node_count", summary.known_node_count);
    InspectionCount(&writer, "unknown_node_count", summary.unknown_node_count);
    InspectionCount(&writer, "missing_required_count", summary.missing_required_count);
    InspectionCount(&writer, "deprecated_count", summary.deprecated_count);
    InspectionCount(&writer, "experimental_count", summary.experimental_count);
    InspectionCount(&writer, "api_node_count", summary.api_node_count);
    InspectionCount(&writer, "observed_output_count", summary.observed_output_count);
    InspectionCount(&writer, "unknown_output_flag_count", summary.unknown_output_flag_count);
    InspectionCount(&writer, "issue_count", summary.issue_count);
    InspectionCount(&writer, "stored_issue_count", summary.stored_issue_count);
    InspectionCount(&writer, "omitted_issue_count", summary.omitted_issue_count);
    UmiJsonWriterRaw(&writer, ",\"issues\":[");
    for (size_t i = 0U; i < summary.stored_issue_count; ++i) {
        UmiComfyPreflightIssue issue;
        status = UmiComfyPreflightIssueAt(report, i, &issue);
        if (status != UMI_STATUS_OK) goto done;
        const char *kind = InspectionKind(issue.kind);
        if (kind == NULL) { status = UMI_STATUS_INVALID_STATE; goto done; }
        if (i != 0U) UmiJsonWriterRaw(&writer, ",");
        UmiJsonWriterRaw(&writer, "{\"kind\":");
        UmiJsonWriterString(&writer, kind);
        UmiJsonWriterRaw(&writer, ",\"node_id\":");
        UmiJsonWriterString(&writer, issue.node_id);
        UmiJsonWriterRaw(&writer, ",\"class_type\":");
        UmiJsonWriterString(&writer, issue.class_type);
        UmiJsonWriterRaw(&writer, ",\"input_name\":");
        UmiJsonWriterString(&writer, issue.input_name);
        UmiJsonWriterRaw(&writer, "}");
    }
    UmiJsonWriterRaw(&writer, "]}");
    status = writer.status;
    if (status == UMI_STATUS_OK) {
        /* Caller buffers never contain a partially formatted finding. The
         * shared writer validates and escapes names; raw calls contain syntax. */
        memcpy(out, staged, writer.length + 1U);
        *out_length = writer.length;
    }
done:
    umi_secret_clear(staged, limit);
    free(staged);
    return status;
}

/* Pagination bounds presentation without changing the complete inventory. The
 * same JSON writer and digest format are shared with presence observations. */
static void ReferenceText(UmiJsonWriter *writer,const char *key,const char *text)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);
    UmiJsonWriterRaw(writer,":");UmiJsonWriterString(writer,text);
}
static void ReferenceBool(UmiJsonWriter *writer,const char *key,bool value)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);
    UmiJsonWriterRaw(writer,":");UmiJsonWriterBool(writer,value);
}
UmiStatus UmiComfyReferenceInspectionJson(const UmiComfyReferenceInventory *inventory,
    size_t offset,size_t limit,char *out,size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL || limit==0U || limit>UMI_COMFY_REFERENCE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(capacity==0U)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfyReferenceSummary summary;
    UmiStatus status=UmiComfyReferenceInventoryInspect(inventory,&summary);
    if(status!=UMI_STATUS_OK)return status;
    if(offset>summary.candidate_count)return UMI_STATUS_NOT_FOUND;
    size_t count=summary.candidate_count-offset;
    if(count>limit)count=limit;
    size_t bound=capacity<UMI_COMFY_REFERENCE_JSON_CAPACITY?capacity:UMI_COMFY_REFERENCE_JSON_CAPACITY;
    char *staged=malloc(bound);
    if(staged==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;UmiJsonWriterInit(&writer,staged,bound);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"reference-candidate-observations\"");
    char decimal[32];
    int written=snprintf(decimal,sizeof decimal,"%" PRIu64,summary.connection_revision);
    if(written<0 || (size_t)written>=sizeof decimal){status=UMI_STATUS_INVALID_STATE;goto done;}
    ReferenceText(&writer,"connection_revision",decimal);
    InspectionDigest(&writer,"connection_sha256",summary.connection_sha256);
    InspectionDigest(&writer,"request_sha256",summary.request_sha256);
    InspectionDigest(&writer,"catalogue_sha256",summary.catalogue_sha256);
    InspectionCount(&writer,"node_count",summary.node_count);
    InspectionCount(&writer,"candidate_count",summary.candidate_count);
    InspectionCount(&writer,"other_array_count",summary.other_array_count);
    InspectionCount(&writer,"unsupported_slot_count",summary.unsupported_slot_count);
    InspectionCount(&writer,"missing_target_count",summary.missing_target_count);
    InspectionCount(&writer,"unobserved_output_count",summary.unobserved_output_count);
    InspectionCount(&writer,"out_of_range_count",summary.out_of_range_count);
    InspectionCount(&writer,"observed_output_count",summary.observed_output_count);
    InspectionCount(&writer,"offset",offset);
    InspectionCount(&writer,"page_count",count);
    InspectionCount(&writer,"remaining_count",summary.candidate_count-offset-count);
    InspectionCount(&writer,"next_offset",offset+count);
    UmiJsonWriterRaw(&writer,",\"candidates\":[");
    for(size_t i=0U;i<count;++i){
        UmiComfyReference candidate;
        status=UmiComfyReferenceInventoryAt(inventory,offset+i,&candidate);
        if(status!=UMI_STATUS_OK)goto done;
        if(i!=0U)UmiJsonWriterRaw(&writer,",");
        UmiJsonWriterRaw(&writer,"{\"node_id\":");UmiJsonWriterString(&writer,candidate.node_id);
        ReferenceText(&writer,"input_name",candidate.input_name);
        ReferenceText(&writer,"target_id",candidate.target_id);
        ReferenceText(&writer,"target_class",candidate.target_class);
        ReferenceText(&writer,"output_type",candidate.output_type);
        ReferenceBool(&writer,"slot_known",candidate.slot_known);
        ReferenceBool(&writer,"target_found",candidate.target_found);
        ReferenceBool(&writer,"output_count_known",candidate.output_count_known);
        ReferenceBool(&writer,"output_observed",candidate.output_observed);
        UmiJsonWriterRaw(&writer,",\"output_slot\":");
        if(candidate.slot_known){
            written=snprintf(decimal,sizeof decimal,"%" PRId64,candidate.output_slot);
            if(written<0 || (size_t)written>=sizeof decimal){status=UMI_STATUS_INVALID_STATE;goto done;}
            UmiJsonWriterString(&writer,decimal);
        }else UmiJsonWriterRaw(&writer,"null");
        UmiJsonWriterRaw(&writer,",\"declared_output_count\":");
        if(candidate.output_count_known)UmiJsonWriterUInt64(&writer,(uint64_t)candidate.declared_output_count);
        else UmiJsonWriterRaw(&writer,"null");
        UmiJsonWriterRaw(&writer,"}");
    }
    UmiJsonWriterRaw(&writer,"]}");
    status=writer.status;
    if(status==UMI_STATUS_OK){memcpy(out,staged,writer.length+1U);*out_length=writer.length;}
done:
    umi_secret_clear(staged,bound);free(staged);return status;
}
