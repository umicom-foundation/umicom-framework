# Reading an Umicom OS boot report

Author: Sammy Hegab, Umicom Foundation. Licence: MIT.

`UmiBootReportParse` belongs to the canonical platform target. It interprets a
bounded report from `umicom-os` without launching a service, performing I/O,
changing a setting or acquiring privileges. The OS boot controller does not
link this reader: its independent writer publishes the protocol below.

```text
UMICOM_BOOT_REPORT 1
mode=normal
state=ready
planned=2
completed=2
reason=none
source=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
```

The example source ID is illustrative. Real IDs are SHA-256 fingerprints of a
captured input manifest; they are not signatures or trusted health attestations.

## Input and lifetime

Pass the byte count, excluding any C string terminator. Maximum size is 512
bytes. A final LF is required. Each of the six fields appears exactly once;
unknown fields, duplicates, controls, unsupported versions and inconsistent
state are rejected. The parser has no locale dependency and allocates nothing.
It copies the result only on success. Input remains caller-owned; no pointer
into it is retained. Distinct calls may run concurrently with independent
output storage. Counts range from zero to 16, with completed <= planned.

`normal` describes the requested route, not the eventual outcome. Its state
may become `recovery` after an error. `recovery` mode means the route was
explicitly requested and never permits a nonzero completed count. Starting
and ready reports require normal mode, a nonempty plan and reason `none`.
Ready requires every planned one-shot check to have completed successfully.
Recovery requires a known failure reason or the explicit `requested` reason.
Normal mode may not use `requested` as its reason.

Known reasons are `none`, `requested`, `configuration`, `mount`, `service-exit`,
`service-timeout`, `service-launch`, `report-write` and `privilege`.

## What ready does not mean

The report is a snapshot of this small boot phase. It does not assert that a
GUI exists, that storage is persistent, that a later application is healthy,
or that a live financial workflow is operational. Readers must not turn the
report into permission to execute privileged operations. Before presenting
boot success, the separate QEMU runner also requires a clean process exit,
init and shutdown markers, matching source identity and the expected service
sequence. The host tests deliberately label synthetic transcripts as fixtures.

The Foundation probe's `--self-test` option exercises canonical memory Data
Server commit and rollback on the host. Its normal guest invocation also
requires a normal, starting boot report. No persistent database is implied.

## Load an explicitly chosen report file

Include `umicom/platform/boot_report_file.h` and link the canonical
`Umicom::platform` target. Call `UmiBootReportLoad(absolutePath, expectedSource,
&report)` from a command-line program or an existing background worker. Pass
`NULL` for `expectedSource` when no source fingerprint is available.

1. Select an absolute regular-file path in a trusted directory. Relative paths,
   a final symbolic link, devices and directories are rejected.
2. Supply the report record itself. A whole serial transcript with init,
   service and shutdown markers is a different format; use the OS image
   transcript assessor for that workflow.
3. Check the returned status before reading output. A successful read is capped
   at 512 bytes and passed to the existing parser. Failed reads, invalid syntax
   and mismatched source identities leave the previous output unchanged.
4. If a source fingerprint was supplied, `UMI_STATUS_INVALID_STATE` means the
   parsed source differed. The expected value must be a lowercase SHA-256 string.
5. Present the parsed state and reason, not just the successful load status.
   Reading a valid recovery/failure report succeeds; it does not make that boot
   successful or establish current machine health.

The reader uses the shared native input-file owner, including UTF-8 Windows
paths and observable same-handle change checks. It never writes the selected
file. Parent aliases may be followed, and a writer that hides metadata changes
is outside this observation contract. The pure parser remains suitable for
borrowed memory without file access; guest PID1 still has no Framework link.
