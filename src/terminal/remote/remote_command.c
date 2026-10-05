/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_command.c
 *
 * PURPOSE:
 *   Implement explicit remote command state without shell concatenation.
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
#include "umicom/terminal/remote/remote_command.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise terminal remote remote command from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_command_init(UmiTerminalRemoteRemoteCommand *value,const char *program,const char *working_directory,bool interactive) { /* Apply this operation only while the related capability or state is available. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this operation only while the related capability or state is available. */ if(program) (void)umi_terminal_remote_copy_text(value->program,sizeof(value->program),program); /* Apply this operation only while the related capability or state is available. */ if(working_directory) (void)umi_terminal_remote_copy_text(value->working_directory,sizeof(value->working_directory),working_directory); value->interactive=interactive; }
/*
 * Check that terminal remote remote command satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_remote_command_valid(const UmiTerminalRemoteRemoteCommand *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->program, '\0', sizeof(value->program)) == NULL) return 0;
    if (memchr(value->working_directory, '\0', sizeof(value->working_directory)) == NULL) return 0;
 return value&&value->program[0]!='\0'&&value->working_directory[0]!='\0'; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTerminalRemoteRemoteCommandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcf175f400f422ff8);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemoteCommand *)0)->program)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemoteCommand *)0)->working_directory)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTerminalRemoteRemoteCommandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTerminalRemoteRemoteCommand *)0)->program) - 1U +
        8U + sizeof(((UmiTerminalRemoteRemoteCommand *)0)->working_directory) - 1U +
        8U;
}
static void UmiTerminalRemoteRemoteCommandArchiveWrite(UmiArchiveWriter *writer, const UmiTerminalRemoteRemoteCommand *value)
{
    UmiArchiveWriteText(writer, value->program, sizeof(value->program));
    UmiArchiveWriteText(writer, value->working_directory, sizeof(value->working_directory));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->interactive);
}
static void UmiTerminalRemoteRemoteCommandArchiveRead(UmiArchiveReader *reader, UmiTerminalRemoteRemoteCommand *value)
{
    UmiArchiveReadText(reader, value->program, sizeof(value->program));
    UmiArchiveReadText(reader, value->working_directory, sizeof(value->working_directory));
    value->interactive = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTerminalRemoteRemoteCommandArchiveValidate(const UmiTerminalRemoteRemoteCommand *value)
{
    return umi_terminal_remote_remote_command_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_terminal_remote_remote_command_archive_encode, umi_terminal_remote_remote_command_archive_decode,
    UmiTerminalRemoteRemoteCommand, UmiTerminalRemoteRemoteCommandArchiveSchema, UmiTerminalRemoteRemoteCommandArchiveBound, UmiTerminalRemoteRemoteCommandArchiveWrite, UmiTerminalRemoteRemoteCommandArchiveRead, UmiTerminalRemoteRemoteCommandArchiveValidate)
