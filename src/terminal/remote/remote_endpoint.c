/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_endpoint.c
 *
 * PURPOSE:
 *   Implement bounded remote endpoint identity and validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_endpoint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise terminal remote remote endpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_endpoint_init(UmiTerminalRemoteRemoteEndpoint *value,const char *host,uint16_t port,bool secure) { /* Apply this branch only when its contract condition is satisfied. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this branch only when its contract condition is satisfied. */ if(host) (void)umi_terminal_remote_copy_text(value->host,sizeof(value->host),host); value->port=port; value->secure=secure; }
/*
 * Check that terminal remote remote endpoint satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_remote_endpoint_valid(const UmiTerminalRemoteRemoteEndpoint *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->host, '\0', sizeof(value->host)) == NULL) return 0;
 return value&&value->host[0]!='\0'&&value->port!=0U; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTerminalRemoteRemoteEndpointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc57952f295410332);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemoteEndpoint *)0)->host)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTerminalRemoteRemoteEndpointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTerminalRemoteRemoteEndpoint *)0)->host) - 1U +
        8U +
        8U;
}
static void UmiTerminalRemoteRemoteEndpointArchiveWrite(UmiArchiveWriter *writer, const UmiTerminalRemoteRemoteEndpoint *value)
{
    UmiArchiveWriteText(writer, value->host, sizeof(value->host));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->port);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->secure);
}
static void UmiTerminalRemoteRemoteEndpointArchiveRead(UmiArchiveReader *reader, UmiTerminalRemoteRemoteEndpoint *value)
{
    UmiArchiveReadText(reader, value->host, sizeof(value->host));
    value->port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->secure = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTerminalRemoteRemoteEndpointArchiveValidate(const UmiTerminalRemoteRemoteEndpoint *value)
{
    return umi_terminal_remote_remote_endpoint_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_terminal_remote_remote_endpoint_archive_encode, umi_terminal_remote_remote_endpoint_archive_decode,
    UmiTerminalRemoteRemoteEndpoint, UmiTerminalRemoteRemoteEndpointArchiveSchema, UmiTerminalRemoteRemoteEndpointArchiveBound, UmiTerminalRemoteRemoteEndpointArchiveWrite, UmiTerminalRemoteRemoteEndpointArchiveRead, UmiTerminalRemoteRemoteEndpointArchiveValidate)
