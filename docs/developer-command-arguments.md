# Parse project commands with Umicom Framework

Project task text is a compatibility format for older task records. New integrations should retain a program and its argument list as separate values when their public API supports that representation. Applications use Framework's parser instead of implementing their own quoting rules.

## Inspect a task without executing it

Include **umicom/developer/command_line.h** and link **Umicom::developer** from a compatible installed SDK:

~~~c
static const char text[] = "compiler -c \"source file.c\" \"\"";
UmiDeveloperCommandLine command = {0};
UmiStatus status = umi_developer_command_line_parse_n(
    text, sizeof(text) - 1U, &command);
if (status != UMI_STATUS_OK) {
    /* Report the invalid task; do not submit an operation. */
    return status;
}
/* command.program is compiler; argument_count is 3.
 * The final argument exists and contains an empty string. */
~~~

The length excludes the terminator. Every byte in the span must be readable. An embedded zero is rejected. The older **umi_developer_command_line_parse** function accepts a valid NUL-terminated string and delegates to the same implementation.

## Grammar and limits

Single and double quotes group bytes. Adjacent quoted and unquoted fragments form one argument. Backslash escapes backslash, either quote, or whitespace; other backslashes remain literal. These are Framework task rules, not a complete shell grammar.

Unquoted pipe, ampersand, semicolon, redirection, parentheses, backtick, newline and carriage return are rejected. Quoted values are literal argument data. Variable, wildcard and command expansion are not performed.

The program must be nonempty. Empty quoted arguments are preserved. The public developer capacity constants define the maximum argument count and storage sizes; each stored string reserves a byte for its terminator. Oversized input returns **UMI_STATUS_CAPACITY_EXCEEDED** rather than truncating a token.

On all errors, the output remains unchanged. Input may overlap the output structure because parsing completes in private staging before publication. Callers must still provide valid readable input and writable output, and synchronize concurrent access themselves.

## Project integration

**umi_developer_task_plan_submit** obtains a task record, finds the command terminator within its fixed field, parses the exact span and then constructs an operation.

**umi_developer_launch_plan_submit** handles a launch profile whose program is already separate from its arguments. It uses a private parser placeholder for the program and passes only the resulting arguments to the chosen run/debug operation.

Neither path submits a partially parsed operation. Application interfaces can retain an invalid draft for correction while keeping the last valid configuration separate.

## Execution policy

Parsing is not authorization. Verify the executable, working directory, environment and intended operation before execution. A task that explicitly starts a shell still requires review of that shell's behaviour. Do not put credentials in command text or diagnostic arguments.

The platform process adapter carries literal argument boundaries to the operating system. Do not join the values back into unescaped shell text in a thin application client.

## Checks and further reading

- **framework.developer.command_spans** covers empty arguments, exact spans, malformed input, capacity boundaries, quoted paths and overlapping input/output.
- **framework.developer.task_plan** checks task translation and rejection before pipeline submission.
- **framework.developer.launch_plan** checks launch arguments and malformed debug requests.
- The Bits2Banking **Learning Build Systems** course includes the **Literal Commands and Safe Build Plans** module and the **labs/framework/command-arguments** SDK exercise.

The parser's prior implementation is retained in a disabled block for review. It inferred token presence from nonzero length and modified caller output before validation. The active parser separates presence from length and publishes only on success. Runtime qualification is required for each consuming platform and SDK.
