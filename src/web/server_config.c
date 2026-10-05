/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_config.c
 *
 * PURPOSE:
 *   Implement default web-server configuration and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/server_config.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Provide the web server config default operation used by this module and its client
 * applications.
 */
UmiWebServerConfig umi_web_server_config_default(void){UmiWebServerConfig c;(void)memset(&c,0,sizeof(c));(void)umi_web_copy_text(c.bind_address,sizeof(c.bind_address),"127.0.0.1");c.port=8080U;c.max_request_bytes=UMI_WEB_BODY_CAPACITY;c.loopback_only=1;return c;}
/* Check that web server config satisfies its contract before another service relies on it. */
UmiStatus umi_web_server_config_validate(const UmiWebServerConfig *config){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->bind_address, '\0', sizeof(config->bind_address)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(config==NULL||config->bind_address[0]=='\0'||config->port==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(config->max_request_bytes==0U||config->max_request_bytes>UMI_WEB_BODY_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWebServerConfigArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x21ba9c77aa15aed7);
    schema = (schema ^ (uint64_t)sizeof(((UmiWebServerConfig *)0)->bind_address)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWebServerConfigArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWebServerConfig *)0)->bind_address) - 1U +
        8U +
        8U +
        8U;
}
static void UmiWebServerConfigArchiveWrite(UmiArchiveWriter *writer, const UmiWebServerConfig *value)
{
    UmiArchiveWriteText(writer, value->bind_address, sizeof(value->bind_address));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->port);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_request_bytes);
    UmiArchiveWriteSigned(writer, (int64_t)value->loopback_only);
}
static void UmiWebServerConfigArchiveRead(UmiArchiveReader *reader, UmiWebServerConfig *value)
{
    UmiArchiveReadText(reader, value->bind_address, sizeof(value->bind_address));
    value->port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->max_request_bytes = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->loopback_only = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiWebServerConfigArchiveValidate(const UmiWebServerConfig *value)
{
    return umi_web_server_config_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_web_server_config_archive_encode, umi_web_server_config_archive_decode,
    UmiWebServerConfig, UmiWebServerConfigArchiveSchema, UmiWebServerConfigArchiveBound, UmiWebServerConfigArchiveWrite, UmiWebServerConfigArchiveRead, UmiWebServerConfigArchiveValidate)
