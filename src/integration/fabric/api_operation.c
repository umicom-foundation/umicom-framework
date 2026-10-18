/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/api_operation.c
 *
 * PURPOSE:
 *   Describe one API operation including method, route, schemas and idempotency semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/integration/fabric/api_operation.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric api operation from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_api_operation_init(UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->method,sizeof(item->method),method);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->path,sizeof(item->path),path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->request_schema,sizeof(item->request_schema),request_schema);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->response_schema,sizeof(item->response_schema),response_schema);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->idempotent=idempotent;
    return umi_fabric_api_operation_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_api_operation_init_build_value(UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->method,sizeof(item->method),method);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->path,sizeof(item->path),path);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->request_schema,sizeof(item->request_schema),request_schema);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->response_schema,sizeof(item->response_schema),response_schema);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->idempotent=idempotent;
    return umi_fabric_api_operation_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_api_operation_init, UmiFabricApiOperation, item,
    (UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent),
    umi_fabric_api_operation_init_build_value(candidate, operation_id, method, path, request_schema, response_schema, idempotent))

/*
 * Check that fabric api operation satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_api_operation_validate(const UmiFabricApiOperation *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation_id, '\0', sizeof(item->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->method, '\0', sizeof(item->method)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->path, '\0', sizeof(item->path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->request_schema, '\0', sizeof(item->request_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->response_schema, '\0', sizeof(item->response_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->operation_id[0]!='\0' && item->method[0]!='\0' && item->path[0]=='/')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricApiOperationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x13ddcb673197c425);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricApiOperation *)0)->operation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricApiOperation *)0)->method)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricApiOperation *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricApiOperation *)0)->request_schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricApiOperation *)0)->response_schema)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricApiOperationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricApiOperation *)0)->operation_id) - 1U +
        8U + sizeof(((UmiFabricApiOperation *)0)->method) - 1U +
        8U + sizeof(((UmiFabricApiOperation *)0)->path) - 1U +
        8U + sizeof(((UmiFabricApiOperation *)0)->request_schema) - 1U +
        8U + sizeof(((UmiFabricApiOperation *)0)->response_schema) - 1U +
        8U;
}
static void UmiFabricApiOperationArchiveWrite(UmiArchiveWriter *writer, const UmiFabricApiOperation *value)
{
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteText(writer, value->method, sizeof(value->method));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->request_schema, sizeof(value->request_schema));
    UmiArchiveWriteText(writer, value->response_schema, sizeof(value->response_schema));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->idempotent);
}
static void UmiFabricApiOperationArchiveRead(UmiArchiveReader *reader, UmiFabricApiOperation *value)
{
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    UmiArchiveReadText(reader, value->method, sizeof(value->method));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->request_schema, sizeof(value->request_schema));
    UmiArchiveReadText(reader, value->response_schema, sizeof(value->response_schema));
    value->idempotent = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFabricApiOperationArchiveValidate(const UmiFabricApiOperation *value)
{
    return umi_fabric_api_operation_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_api_operation_archive_encode, umi_fabric_api_operation_archive_decode,
    UmiFabricApiOperation, UmiFabricApiOperationArchiveSchema, UmiFabricApiOperationArchiveBound, UmiFabricApiOperationArchiveWrite, UmiFabricApiOperationArchiveRead, UmiFabricApiOperationArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_fabric_api_operation_archive_decode_with_workspace, UmiFabricApiOperation,
    UmiFabricApiOperationArchiveSchema, UmiFabricApiOperationArchiveBound, UmiFabricApiOperationArchiveRead, UmiFabricApiOperationArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_fabric_api_operation_archive_decode_if_unchanged, umi_fabric_api_operation_archive_encode,
    UmiFabricApiOperation, UmiFabricApiOperationArchiveSchema, UmiFabricApiOperationArchiveBound, UmiFabricApiOperationArchiveRead, UmiFabricApiOperationArchiveValidate)
