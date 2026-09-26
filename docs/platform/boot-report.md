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
