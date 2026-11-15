![Umicom](../../resources/brand/umicom-logo.svg)

# Build a native CodeGuard client from the installed SDK

This independent C23 example links the exported Umicom::codeguard target.
Framework owns scanning rules, result storage, reports and severity policy.
The application owns one service and checks each operation. It does not clone
Framework, embed its source or invoke an external analyser.

## Prepare and build

1. Obtain a compatible installed Framework SDK that includes CodeGuard and the
   current service implementation. Headers alone are insufficient; an older
   library with the same package version may not contain a newer API or repair.
2. Use the compiler, architecture, runtime and ABI recorded in that package.
   The example requires an explicit existing absolute SDK prefix.
3. Open this example directory or copy it to your application workspace.
4. Run each command separately, substituting your actual SDK prefix and a small
   disposable practice source directory. Stop on an unexpected error.

~~~powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUMICOM_SDK_ROOT=C:/SDKs/Umicom
cmake --build build --parallel 2
& ".\build\umicom-sdk-codeguard.exe" "C:\Practice\quality-demo"
$LASTEXITCODE
~~~

An exit of 0 means the scan and report completed and the selected severity gate
did not fail. An exit of 1 means the scan completed but findings failed the gate.
An exit of 2 means usage or service creation failed. An exit of 3 means scanning
or report output failed. A completed heuristic scan does not prove that source
is safe or correct. Review findings and run the application's real checks.

The package's shared branding helper stages the approved SVG resources and
Windows ICO resource. No font downloads or replacement logos are introduced.
Build, runtime dependency deployment and icon appearance require qualification
on the target platform.

## Retain reports in a longer-lived application

The example scans once, but a Studio-style host can retain the service across
rescans. A failed rescan now preserves the previous complete result. Display
the failed attempt separately; do not relabel retained findings as a new pass.
A successful empty scan replaces old findings with an empty result.

The borrowed result object's address stays valid until service destruction.
Pointers to individual findings survive a failed scan, but a successful
replacement invalidates them. Serialize access; this synchronous service does
<!-- Earlier ordinary-service limitation retained: not create a worker, cancel directory traversal or synchronize UI callbacks. -->
not create a worker or synchronize UI callbacks. The cancellable entry point observes a shared token during traversal and between analysis phases.

Roots are copied exactly up to 1023 bytes. Empty and longer roots refuse
creation. Prefer an absolute root to avoid dependence on the scan caller's
current directory. Directory existence is established by scanning, not by
service allocation.

Read [the ownership lesson](../../docs/learning/codeguard-owned-results.html)
for allocation failures, aliasing and the focused regression workflow.
Installed-consumer compilation and execution must be checked with your actual
SDK; source availability alone is not a successful package qualification.

## Observe cancellation without replacing a report

Pass --cancel-before-scan after the root to exercise deterministic preflight cancellation. The example creates a Framework token, requests it before calling the synchronous service, waits for that call to return, then destroys the token. It prints a cancelled-attempt message and returns **4**, without scanning the directory or printing a new report.

~~~powershell
& ".\build\umicom-sdk-codeguard.exe" "C:\Practice\quality-demo" --cancel-before-scan
$LASTEXITCODE
~~~

A longer-lived host can request the same token from its existing worker coordinator. Keep the token and service alive until the scan returns. Do not reset a token during a scan or destroy it immediately after requesting cancellation. Requesting is thread-safe; service access and finding borrows still require serialization. A request after the final publication check may arrive too late to cancel an already successful scan.

Read [cooperative CodeGuard cancellation](../../docs/learning/codeguard-cancellation.html) for boundaries, cleanup, error precedence and focused checks. Cancellation does not interrupt an operating-system call or analysis pass already in progress. This example remains an installed-SDK consumer requiring qualification with the actual package.

## Distinguish incomplete analysis from findings

Finding insertion, lifetime tracker capacity and source read/close failures are
scan errors. The service discards a failed candidate and retains its previous
complete report. This one-shot example returns 3 for these failures; it must not
print a fresh successful report based on partial findings.

The checked lifetime function returns status; the older void entry point remains
for compatibility. Independent consumers must use headers and libraries from the
same qualified SDK. Read [checked analysis failures](../../docs/learning/codeguard-analysis-failures.html)
for the 64-name tracker limit, transactional measurement output, fault injection
and the remaining textual-analysis limitations.

## Choose the workspace boundary

Recursive exclusions are evaluated below the root you select. An ancestor named
build does not hide your project's source. A build directory inside the project
remains excluded; explicitly selecting that inner directory makes it the new
root. Framework keeps the visited file address for reading and diagnostics.

Template/workspace directories below the root remain scanned, but their payloads
are not duplicate candidates. The direct-file API bypasses recursive selection.
Read [workspace-relative paths](../../docs/learning/codeguard-workspace-paths.html)
for component matching, platform case rules, Unicode limits and real-scanner
fixtures. This policy is not a filesystem sandbox and does not prune traversal.

## Review duplicate candidates before consolidation

Duplicate findings report raw fingerprint plus size matches, or normalized
fingerprint matches. They are candidates for review, not proof of byte identity
or semantic equivalence. The explicit duplicate-review marker has its own
message and does not imply that any pair of files was compared.

Collection and finding-storage failures abort the scan. A retained service keeps
its previous report rather than presenting incomplete duplicate evidence as a
completed attempt. Read [duplicate evidence](../../docs/learning/codeguard-duplicate-evidence.html)
for checked growth, input ownership, failure tests and normalization limits.

## Select source categories explicitly

Set the C, C++ and header flags on UmiCodeGuardConfig before creating a service.
The shared scanner applies them to direct-file analysis and duplicate admission.
The generated-source flag opts into build and _CPack_Packages children while
vendor and rule directories remain excluded. Filename policy is independent.

<!-- Earlier contract retained for review: The max_file_bytes field is not yet enforced by the current readers; it must
not be treated as an input budget. -->
The max_file_bytes field now limits each native file read in profile-driven
scanning, including later architecture and duplicate passes. Zero admits empty
files only; an oversized file fails the scan instead of becoming a successful
skip. This does not limit total workspace memory, findings or CPU work. Read [scan profiles](../../docs/learning/codeguard-scan-profiles.html)
for the supported behavior, fixtures and precise pattern-insertion failures.

## Extend rules with explicit ownership

Custom registry entries copy descriptors while borrowing their strings. Keep
pattern and diagnostic text alive until the registry is destroyed. A valid
borrowed descriptor range can be appended back to the same registry; successful
growth invalidates previous descriptor pointers. A failed add leaves the registry
unchanged. Builtin loading remains a sequence of groups, so discard a failed
candidate registry rather than presenting it as a complete rule set.

Read [rule registry ownership](../../docs/learning/codeguard-rule-registry.html)
for checked growth, missing-pattern refusal, failure injection and SDK lifetime
examples. The ordinary SDK scan example still uses the shared builtin catalogue.

## Use bounded input outside a full scan

The installed source_measure_bounded and hash_file_bounded APIs accept an
explicit byte budget and a borrowed cancellation token. They preserve caller
outputs on failure. The old standalone stdio APIs remain available; choose the
bounded variants for native UTF-8 paths and regular final-file admission.
Final links are refused, and source text containing NUL is rejected. Binary
hashing still accepts every byte value. Profile-driven scans already select the
bounded path through Framework, so applications do not need a second reader.

Read [bounded source analysis](../../docs/learning/codeguard-bounded-input.html)
for exact symbols, ownership, relative-path resolution and qualification. Each
pass opens and checks its own file; metadata checks do not create a snapshot.

## Run the background SDK example

The same example project also builds umicom-sdk-codeguard-background. It uses
the exported job and task queue services, copies the profile and freezes the
absolute root before dispatch. Framework owns scanning and candidate findings;
the console takes one complete result after draining its private queue.

~~~powershell
& ".\build\umicom-sdk-codeguard-background.exe" "C:\Practice\quality-demo"
$LASTEXITCODE
& ".\build\umicom-sdk-codeguard-background.exe" "C:\Practice\quality-demo" --cancel-before-submit
$LASTEXITCODE
~~~

The exit meanings remain 0 for a completed passing gate, 1 for a completed
failing gate, 2 for usage/setup failure, 3 for an operational/report failure and
4 for cancellation. The cancellation example starts no scan. A completed scan
does not prove the source is safe. This new target requires an installed SDK
containing the job implementation as well as its header; qualification against
an actual installed package remains necessary.

A GUI host polls UmiCodeGuardJobRead on its existing event loop instead of using
this console's blocking queue shutdown. Take and destruction remain on the
creating thread. Keep the previous report until a successful result transfer,
and never release a running job merely because cancellation was requested.
Read [background scan ownership](../../docs/learning/codeguard-background-jobs.html)
for effective versus original status, queue rejection and the publication boundary.

## Adopt into a retained service

Add --adopt to the background example to prepare the job from a Framework
service and attach its completed findings through the checked adoption API.
The default mode still takes an independently owned result. Both modes accept
--cancel-before-submit; cancellation returns 4 without publishing a new report.

~~~powershell
& ".\build\umicom-sdk-codeguard-background.exe" "C:\Practice\quality-demo" --adopt
$LASTEXITCODE
~~~

The retained result is borrowed from its service and must not be freed by the
report writer. Adoption checks the resolved workspace before consuming the
one-shot job. It preserves old findings on failure, keeps the result object
address on success and invalidates old finding borrows when replacing storage.
The caller chooses which same-workspace job is current; no automatic newest-job
ordering is provided. See [workspace-checked adoption](../../docs/learning/codeguard-job-adoption.html)
for Studio facade integration, relative-root limits and qualification commands.

## Keep a current review context

The project also builds umicom-sdk-codeguard-review. This example uses an
absolute root and a host context token, then reports task execution separately
from publication. It accepts --cancel-before-publication or
--stale-before-publication to demonstrate a result completed by the worker
but suppressed before it reaches the retained report.

~~~powershell
& ".\build\umicom-sdk-codeguard-review.exe" "C:\Practice\quality-demo"
$LASTEXITCODE
& ".\build\umicom-sdk-codeguard-review.exe" "C:\Practice\quality-demo" --stale-before-publication
$LASTEXITCODE
~~~

For a scan that completes normally, cancellation returns 4 and a stale context
returns 5. Earlier operational failures remain errors; a report gate still
returns 0 or 1, usage/setup returns 2 and other failures return 3. The console
joins its private queue for a deterministic demonstration; a GUI must poll its
existing event loop and preserve its shared queue. No GUI command is installed
by this example. Read [current-request ownership](../../docs/learning/codeguard-current-request.html)
for host token rules, borrowed results and retirement retries.

## Export a bounded report projection

The report example creates owned UTF-8 text, JSON or SARIF through Framework.
It then releases the scan service before using the returned bytes. This makes
the difference between a borrowed finding and an owned presentation explicit.
Its one MiB payload budget rejects a larger projection before opening an output
file. The existing stream writer remains available for streaming large reports.

~~~powershell
cmake --build build --target umicom-sdk-codeguard-report
& ".\build\umicom-sdk-codeguard-report.exe" "C:\Practice\quality-demo" sarif ".\quality.sarif"
$LASTEXITCODE
& ".\build\umicom-sdk-codeguard-report.exe" "C:\Practice\quality-demo" json
$LASTEXITCODE
~~~

Run this in the separately configured example project using matching installed
headers and libraries. Exit 0 or 1 means output completed and the severity gate
passed or failed, respectively. Exit 2 is usage/setup failure; exit 3 is a scan,
capacity, allocation or output failure. A failing gate is different from an
unwritten report. Output-file mode overwrites an existing file, creates no
missing parent directories and is not an atomic replacement. An I/O failure can
leave partial output. Use an owned destination and review errors before importing it.

Read [report output contracts](../../docs/learning/codeguard-reports.html) for
control-character escaping, SARIF paths, unknown locations, Unicode filenames,
the rendering budget and owner qualification. A valid report is a representation
of scan findings, not proof that the source is safe.


## Save a complete report as a new file

The export example streams through Framework's native exclusive file owner.
It measures the complete serialized result first and refuses reports over
64 MiB before creating a destination. Its fixed staging buffer does not hold
a second complete report. The result must remain alive and unchanged until
the synchronous export finishes.

~~~powershell
cmake --build build --target umicom-sdk-codeguard-export
& ".\build\umicom-sdk-codeguard-export.exe" "C:\Practice\quality-demo" sarif "C:\Practice\reports\quality-new.sarif"
$LASTEXITCODE
~~~

Create and review the parent directory beforehand. The absolute destination
must be a new ordinary file. Repeating the command with that filename refuses
it; use another name to preserve the first report. Native Unicode argument
capture supplies UTF-8 paths on Windows. The existing report example retains
its separate overwrite behavior.

Exit 0 or 1 means export completed and the severity gate passed or failed.
Exit 2 means usage or setup failure; exit 3 means scanning, report capacity,
output, or completion-message failure. An I/O failure after creation can leave
a partial new file, which is retained for diagnosis. Exclusive creation does
not hide a report from concurrent readers while it is being written and does
not guarantee survival of power loss. A GUI should run storage I/O on a worker
with its own stable report, not borrow a changing service result on that worker.

Read [exclusive report export](../../docs/learning/codeguard-exclusive-export.html)
for the shared API, the umicom --output-new option, error precedence and
regression qualification. Installed SDK compilation and execution of this
example still need qualification with matching headers and libraries.
