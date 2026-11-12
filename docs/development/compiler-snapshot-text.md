![Umicom](../../resources/brand/umicom-logo.svg)

# Format a complete compiler snapshot

A compiler snapshot summarizes recorded providers, profiles, projects and commands. Native clients can present the same summary through **umi_compiler_centre_format_snapshot**. The function formats metadata and does not discover or execute tools.

## Keep the previous view until a replacement is complete

1. Obtain a snapshot through **umi_compiler_centre_snapshot_checked** and check its result.
2. Supply a writable text buffer separate from the snapshot.
3. Call the formatter and publish the text only after **UMI_STATUS_OK**.
4. On refusal, keep the previous view and report the actual status.

~~~c
char text[1024];
UmiStatus status = umi_compiler_centre_format_snapshot(snapshot, text, sizeof(text));
if (status != UMI_STATUS_OK) return status;
/* The complete text can now be presented by the caller. */
~~~

This function-body fragment assumes an existing stable snapshot pointer and includes **umicom/compiler/centre.h**. It does not initialize the snapshot or protect it against concurrent writers.

The formatter checks count bounds, available-provider count and active-profile string termination before formatting into a local candidate. It then checks the receiving capacity, including the terminating zero. A short buffer returns **CAPACITY_EXCEEDED** without replacing any destination byte. Malformed metadata returns **INVALID_STATE**. Missing pointers, zero capacity or overlap with the snapshot return **INVALID_ARGUMENT**.

Success copies the complete text and its terminator; unused trailing destination bytes remain untouched. A saturated revision does not prevent this read-only presentation. Neither a valid snapshot nor an “available” provider label establishes that a real executable remains installed or can build the requested target.

## Verification and recovery

The authored **framework.compiler.snapshot_format** regression covers exact text, exact-fit and short buffers, malformed text/counts, source/output overlap and read-only use of a saturated revision. It remains unexecuted until qualified against the actual build.

If a client receives a capacity refusal, choose enough presentation storage and retry with the same stable snapshot. Do not accept a previously truncated prefix as the new view. For malformed metadata, inspect the snapshot acquisition and owner lifetime. Increasing the output buffer cannot repair damaged source records.
