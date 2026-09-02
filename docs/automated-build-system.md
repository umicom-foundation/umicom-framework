# Umicom Automated Build System

The Umicom Automated Build System is the Framework decision layer that turns
changed files into a small, safe build and test plan. It keeps the existing
compiler toolchain: CMake describes the project, Ninja performs dependency-aware
incremental compilation, and CTest runs the selected tests. The new layer decides
which of those actions are necessary, so a developer does not need to name a
module or remember a test expression.

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

Build affected products and run their focused tests:

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
commit, push or delete source files.

## Continuous local integration

Start one workspace controller and leave it running while any combination of
Umicom applications is in use:

```text
umicom automate watch C:\umicom\umicom-applications --preset windows-ucrt64-debug --jobs 2
```

The controller establishes a file baseline, seeds changes that were already in
the working tree, and then watches recursively. A short quiet period groups the
several file writes made by one editor save. Generated build, package, install
and local runtime paths are ignored, which prevents a completed build from
starting itself again.

For each quiet batch the controller performs the following lifecycle:

1. normalise and deduplicate changed paths;
2. calculate direct module ownership and shared Framework consumers;
3. configure only when build definitions or manifests require it;
4. build affected aggregate targets and compile their test targets;
5. run the focused application and integration tests;
6. optionally stage an installation when `--deploy` was explicitly supplied;
7. publish version, build target and generation metadata only after success.

Use `--debounce MS` to change the quiet period and `--interval MS` to change how
often the portable watcher scans. `--ignore-existing` omits working-tree changes
made before startup. `--all` requests one complete initial generation and then
continues watching. Ctrl+C asks the controller to stop at a safe boundary.

Compiler discovery happens before the first non-empty generation and is reused
afterwards. Native incremental compilation remains the responsibility of the
configured build tool, which already recompiles an object only when its source,
included headers or build rules changed.

A failed generation never publishes an update and its changed-file evidence is
retained. The next source edit retries that failed slice together with the new
change, while avoiding an uncontrolled retry loop when nobody has changed the
code.

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

Configuration creates the following names from checked-in application manifests:

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

`umicom/build/automation.h` and `umicom/build/continuous_integration.h` are
toolkit-neutral. File watchers, Umicom Studio,
continuous-integration workers and future build dashboards can all use the same
contract:

- initialise and register scopes;
- add changed-path evidence;
- evaluate direct and inherited impact;
- read copied plan items and aggregate snapshots;
- display stable action and change-kind descriptions.

The continuous contract adds debounce state, generation counters, copied plan
items and a synchronous update sink. The command-line host composes it with the
Framework watcher, clock, build executor and durable change broker. Applications
can host the same contract without copying the planning rules.

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
