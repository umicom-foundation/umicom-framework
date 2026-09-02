# Umicom Automated Build System

The Umicom Automated Build System is the Framework decision layer that turns
changed files into a small, safe build and test plan. It keeps the existing
configured compiler, incremental builder and test runner. The new layer decides
which actions are necessary, so a developer does not need to name a module or
remember a test expression.

## How it works

1. The Framework version-control provider reads changed and untracked paths.
2. Application manifests are discovered below the `applications` directory.
3. Each path is classified as source, header, assembly, build definition, test,
   resource, documentation, manifest or generated input.
4. The longest matching product scope owns the direct change.
5. Shared Framework changes select registered application consumers. Root build
   definition changes select the whole configured workspace because they can
   alter any dependency.
6. The plan is printed before execution. Duplicate product and test targets are
   merged, and targets run in deterministic order.

Documentation-only changes do not start compilation. Resource changes rebuild
their owning product so embedded or staged resources stay current. A build
definition or application manifest change runs configuration before compilation.

## Commands

Inspect the plan without running a compiler or test:

```text
umicom automate plan C:\umicom\umicom-applications
```

Planning prepares only the repository-read capability. Compiler discovery and
environment preparation begin only for `run`, after a non-empty plan exists,
and are reused for every selected target in that run.

Verify changed source, build affected products and run their focused tests:

```text
umicom automate run C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2
```

Build without tests while diagnosing a compilation problem:

```text
umicom automate run C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2 --no-tests
```

Request a complete configured-product and test pass:

```text
umicom automate run C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2 --all
```

Install only after every selected build and test action succeeds:

```text
umicom automate run C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2 --deploy --prefix C:\umicom\installed
```

Deployment is deliberately opt-in because installation changes files outside the
build directory. Planning and ordinary automated builds never install, publish,
commit, push or delete source files. Deployment cannot be combined with
`--no-tests`; installed artifacts must have focused executable-test evidence.

## Continuous local integration

Start one workspace controller and leave it running while any combination of
Umicom applications is in use:

```text
umicom automate watch C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2
```

The controller establishes a file baseline, seeds changes that were already in
the working tree, and then watches recursively. Generated build, package,
install and local runtime paths are ignored, which prevents a completed build
from starting itself again.

The default lifecycle deliberately separates checking source from compiling it:

1. each save normalises and deduplicates its changed path;
2. ten quiet minutes after the last save, changed C and header files pass the
   local security and quality gate;
3. a new save invalidates older verification and restarts the quiet timer;
4. after successful verification, the unchanged revision waits twenty minutes;
5. the Framework calculates direct module ownership and shared consumers;
6. affected targets compile and their focused executable tests run;
7. an installation is staged only when local policy or `--deploy` enables it;
8. version, build target and generation metadata publish only after success.

The latest machine-readable quality evidence is stored in the untracked
`.umicom/runtime/quality-latest.sarif` file. A failed gate blocks compilation,
keeps the changed-file evidence and waits for a repair. No source file is
deleted or rewritten by the controller.

Use `--interval MS` to change how often the portable watcher scans.
`--ignore-existing` omits working-tree changes made before startup. `--all`
requests one complete initial generation and then continues watching. Ctrl+C
asks the controller to stop at a safe boundary.

Compiler discovery happens before the first non-empty generation and is reused
afterwards. Native incremental compilation remains the responsibility of the
configured build tool, which already recompiles an object only when its source,
included headers or build rules changed.

A failed generation never publishes an update and its changed-file evidence is
retained. The next source edit retries that failed slice together with the new
change, while avoiding an uncontrolled retry loop when nobody has changed the
code.

## Local timing configuration

Scheduling uses the ignored local file `.umicom/automation.conf`. A missing
file is valid and uses Framework defaults. The file is not committed because
different developers and build machines may use different working hours.

```text
# Wait for active editing to become quiet before checking changed source.
verification_quiet_minutes=10

# Wait after a successful check before compiling the unchanged revision.
build_delay_minutes=20

# Prevent an ordinary quiet-time generation from being forgotten.
watchdog_minutes=60

# Zero uses the quiet-time policy. A positive value creates a build interval.
build_interval_minutes=0

# Automatic work can be paused without disabling manual requests.
automatic_builds=true

# Installation remains disabled unless a developer explicitly enables it.
automatic_deploy=false
```

The parser rejects unknown keys, incomplete lines, invalid booleans and numeric
overflow. It applies the whole file or none of it, preventing a partly loaded
safety policy. Inspect the effective policy without starting a compiler:

```text
umicom automate settings C:\umicom\umicom-applications
```

For a controller that collects approved work and runs at eight-hour intervals,
set:

```text
build_interval_minutes=480
automatic_deploy=true
```

Interval mode still verifies changed source after its quiet period, but holds
the approved revision until the next interval boundary. The normal watchdog
does not force an interval build to run early.

When development finishes sooner, request the pending generation immediately:

```text
umicom automate trigger C:\umicom\umicom-applications
```

The request wakes the already running controller. It cancels timing waits but
does not bypass source verification, compilation, executable tests or
deployment policy. The request marker remains in the ignored runtime folder as
local evidence; the controller remembers its modification time and does not
replay an old request after restart.

## Discovering module updates

Successful native builds are announced on the stable topic
`umicom.build.module-update.available`. Records are appended to the local,
untracked `.umicom/runtime/module-updates.journal` file. Each record contains the
module identifier, display name, manifest version, build target, generation and
restart policy. The broker resumes its sequence from the existing journal after
a controller restart, so newer records always have a larger checkpoint.

An application uses the shared Framework message broker rather than reading the
file format itself:

1. create a durable change broker for the workspace journal;
2. subscribe to `UMI_BUILD_CONTINUOUS_UPDATE_TOPIC`;
3. call `umi_change_broker_replay_after` with its last saved sequence;
4. show an update notification when a relevant module generation is newer;
5. save the returned checkpoint and poll again later.

The replay-after operation delivers only unseen records, making it suitable for
a background status service shared by Studio, Trader, Desk and other clients.
Native libraries and executables are marked `restart_required`. A running
process must not unload code while objects or callbacks may still point into it;
the application can save its layout and session, then activate the new module on
a controlled restart. Data-only reflected resources may use a separate validated
live-reload policy in the future.

## Stable aggregate targets

Workspace preparation creates the following names from checked-in application
manifests:

- `umicom-products` builds the configured application entry points and native
  Framework command.
- `umicom-product-<application>` builds one configured application entry point.
- `umicom-tests` compiles the complete configured test estate when a full pass is
  explicitly required.
- `umicom-tests-framework` compiles Framework-owned tests.
- `umicom-tests-<application>` compiles tests declared by one application.
- `umicom-tests-integration` compiles the bounded root tests that verify shared
  composition contracts. Focused Framework and application targets include it.

The application aggregates choose the available graphical entry point first and
fall back to the manifest or console entry point. This is why the same automated
command works with graphical and headless presets. Focused application test
expressions run the affected scope plus the shared `applications.*` integration
contracts. A compilable Framework change runs the complete configured test
estate because a public contract can affect every enabled consumer.

## Framework API

`umicom/build/automation.h`, `umicom/build/automation_schedule.h` and
`umicom/build/continuous_integration.h` are toolkit-neutral. File watchers,
Umicom Studio, continuous-integration workers and future build dashboards can
all use the same contract:

- initialise and register scopes;
- add changed-path evidence;
- evaluate direct and inherited impact;
- read copied plan items and aggregate snapshots;
- display stable action and change-kind descriptions.

The schedule contract adds quiet-time, watchdog, recurring interval, manual
request and verification/build phases. The continuous contract adds generation
counters, copied plan items and a synchronous update sink. The command-line host
composes them with the Framework watcher, clock, quality scanner, build executor
and durable change broker. Applications can host the same contracts without
copying policy.

The API does not execute commands. This separation makes planning independently
testable and prevents a user-interface client from bypassing toolchain safety,
timeouts, cancellation or hidden-process policy.

## Conservative safety rules

An unknown changed file uses its scope's default build-and-test policy rather than
being silently ignored. A dirty Framework submodule may appear to the parent
repository as the single path `framework`; that still selects every Framework
consumer safely. A future optimisation can inspect nested repository changes to
narrow this case, but it must never miss a client affected by a public contract.

Generated aggregate targets replace false build dependencies between independent
applications. For example, rebuilding the application launcher no longer forces
the IDE, trading, banking and treasury executables to build merely because they
can be launched at runtime.
