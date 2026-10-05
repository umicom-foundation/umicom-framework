/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_port_forward.c
 *
 * PURPOSE:
 *   Implement port-forward validation.
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
#include "umicom/terminal/remote/remote_port_forward.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise terminal remote remote port forward from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_port_forward_init(UmiTerminalRemoteRemotePortForward *value,uint16_t local_port,const char *remote_host,uint16_t remote_port) { /* Apply this operation only while the related capability or state is available. */ if(!value) return; (void)memset(value,0,sizeof(*value)); value->local_port=local_port; value->remote_port=remote_port; /* Apply this operation only while the related capability or state is available. */ if(remote_host) (void)umi_terminal_remote_copy_text(value->remote_host,sizeof(value->remote_host),remote_host); value->enabled=true; }
/*
 * Check that terminal remote remote port forward satisfies its contract before another
 * service relies on it.
 */
bool umi_terminal_remote_remote_port_forward_valid(const UmiTerminalRemoteRemotePortForward *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->remote_host, '\0', sizeof(value->remote_host)) == NULL) return 0;
 return value&&value->enabled&&value->local_port!=0U&&value->remote_port!=0U&&value->remote_host[0]!='\0'; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTerminalRemoteRemotePortForwardArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x43af317bde8a3e11);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemotePortForward *)0)->remote_host)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTerminalRemoteRemotePortForwardArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiTerminalRemoteRemotePortForward *)0)->remote_host) - 1U +
        8U;
}
static void UmiTerminalRemoteRemotePortForwardArchiveWrite(UmiArchiveWriter *writer, const UmiTerminalRemoteRemotePortForward *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->local_port);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->remote_port);
    UmiArchiveWriteText(writer, value->remote_host, sizeof(value->remote_host));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiTerminalRemoteRemotePortForwardArchiveRead(UmiArchiveReader *reader, UmiTerminalRemoteRemotePortForward *value)
{
    value->local_port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->remote_port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    UmiArchiveReadText(reader, value->remote_host, sizeof(value->remote_host));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTerminalRemoteRemotePortForwardArchiveValidate(const UmiTerminalRemoteRemotePortForward *value)
{
    return umi_terminal_remote_remote_port_forward_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_terminal_remote_remote_port_forward_archive_encode, umi_terminal_remote_remote_port_forward_archive_decode,
    UmiTerminalRemoteRemotePortForward, UmiTerminalRemoteRemotePortForwardArchiveSchema, UmiTerminalRemoteRemotePortForwardArchiveBound, UmiTerminalRemoteRemotePortForwardArchiveWrite, UmiTerminalRemoteRemotePortForwardArchiveRead, UmiTerminalRemoteRemotePortForwardArchiveValidate)
