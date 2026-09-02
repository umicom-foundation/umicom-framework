# Reflection and Dynamic Discovery

Umicom Framework reflection is a searchable description of code and resources.
It lets an application answer questions such as:

- Which modules are available?
- Which fields belong to a type?
- Which fields contain private information and must be hidden?
- Which layouts, panels, commands, schemas and themes are installed?
- Which module contributed a resource?
- What changed while the application was running?

The Framework does not inspect arbitrary process memory. C compilers are
allowed to remove source names and type relationships, so attempting to guess
them at runtime would be unreliable. Instead, modules provide explicit
metadata that the Framework validates and copies into owned storage.

## Why this is an engine rather than a required server

`UmiReflectionEngine` runs inside an application. This keeps type inspection,
layout discovery and property editing available when the computer is offline.
It also avoids opening a network port merely to inspect the current process.

An external Reflection Server may be added later as an adapter. It could expose
approved, redacted snapshots from several applications, but it would still ask
each application's Reflection Engine for its information. The engine remains
the source of truth.

## Main building blocks

### Reflection Registry

The registry owns copies of type, module and resource metadata. Plug-ins may be
unloaded after their reflected records are removed without leaving pointers to
released plug-in memory.

The registry stores:

- type descriptions and safe field boundaries;
- module identity, ABI, version and public capabilities;
- resources such as layouts, panels, commands, schemas and themes;
- a monotonic revision changed by every successful mutation.

Queries return identifiers or caller-owned snapshots. They do not return a
mutable pointer into registry storage.

### Reflection Engine

The engine coordinates registry changes and notifications. It can project the
descriptive part of an existing `UmiModuleDescriptor` while deliberately
excluding lifecycle functions and private module-state pointers.

When durable notification is required, registration is rolled back if the
change cannot be recorded. A caller therefore does not receive a successful
answer for metadata that was supposed to be durable but was not journalled.

### Change Broker

The Change Broker composes the existing Umicom event bus and append-only event
journal. It is a small embedded publish/subscribe service, not a duplicate
messaging system.

With no journal path it works entirely in memory. With a journal path it:

1. validates the change event;
2. appends the event to the journal;
3. flushes the journal;
4. publishes the event to subscribers.

This order means a subscriber never observes a change that failed to reach its
required journal. The journal can be replayed to rebuild projections or help a
developer understand what changed.

The existing Umicom messaging package continues to provide commands, queries,
topics, routing, inboxes, outboxes, dead letters, delivery leases, workflows
and reliability policies. Applications should use those facilities when they
need a distributed or remotely transported message service.

## Declaring a reflected C type

The compiler should calculate field offsets and sizes. Developers should not
write those numbers by hand.

```c
typedef struct ExampleProject {
    uint32_t identifier;
    char name[64];
} ExampleProject;

static const UmiReflectionFieldDescriptor project_fields[] = {
    UMI_REFLECTION_FIELD(
        ExampleProject,
        identifier,
        "c.uint32",
        UMI_REFLECTION_VALUE_UNSIGNED_INTEGER,
        UMI_REFLECTION_FIELD_REQUIRED,
        "Stable project identifier."),
    UMI_REFLECTION_ARRAY_FIELD(
        ExampleProject,
        name,
        "c.char",
        UMI_REFLECTION_VALUE_TEXT,
        UMI_REFLECTION_FIELD_REQUIRED,
        "Name displayed to the user.")
};
```

The type descriptor then supplies the type identity, owning module, complete
structure size, alignment and the field array. Registration rejects fields
that extend beyond the structure, duplicate field names, unterminated text and
unsupported metadata counts.

## Discovering modules without rebuilding the host

Executable modules use the established Umicom plug-in flow:

1. `umi_plugin_discover` finds inspectable manifests without running code.
2. Trust, permission and compatibility policies examine the manifest.
3. `umi_plugin_loader_load` loads an approved dynamic library.
4. The library returns its stable `UmiModuleDescriptor`.
5. `umi_reflection_engine_register_runtime_module` copies safe module metadata.
6. The application's normal module controller manages its lifecycle.

Reflection does not load libraries and does not grant permissions. Those jobs
remain with the plug-in and security services.

Unloading follows the reverse order: stop the module, unregister its reflected
types and resources, unregister its reflected module description, and then
unload the dynamic library.

## Adding layouts without rebuilding an application

Layouts are portable `.umilayout` documents rather than compiled toolkit
widgets. `umi_workbench_layout_discover` walks a configured directory and:

1. ignores unrelated files;
2. enforces a maximum file size;
3. decodes and validates each semantic layout document;
4. adds or refreshes its layout template;
5. adds or refreshes its reflected layout resource;
6. publishes a resource registration or change event.

An application can connect the established filesystem watcher to this
operation. When a layout file changes, the watcher asks discovery to load that
file again. Layout selectors can then query the updated template registry and
refresh their visible choices without restarting the application.

Toolkit adapters still decide how semantic nodes become native widgets. A
layout document never serialises widget pointers.

## Change topics

The Reflection Engine publishes exact topics for:

- type registration and removal;
- module registration and removal;
- resource registration, refresh and removal.

Payloads contain only stable identifiers, the owning module identifier and the
registry revision. Reflected object contents are not placed on the event bus.
This keeps secrets and large data structures out of journals.

## Safety and privacy rules

- Mark passwords, tokens and private values with
  `UMI_REFLECTION_FIELD_SENSITIVE`.
- Inspectors and AI context builders must redact sensitive fields by default.
- Never serialise the raw bytes of an entire structure.
- Never invoke a function through an incorrectly typed function pointer.
- Use generated, type-safe wrappers if callable reflection is introduced.
- Validate a module before accepting its reflected metadata.
- Remove reflected records before unloading their contributing module.
- Apply file-size, capacity, trust and permission limits to discovery.
- Keep executable loading separate from data-only layout discovery.

## Assembly code

Assembly is not needed to discover C field names or type relationships because
those details may no longer exist in a compiled binary. The portable metadata
registry therefore remains authoritative.

Small architecture-specific helpers may be added later for measured operations
such as call adaptation or metadata scanning. Each helper must have a portable
C implementation, focused tests and an audited calling convention. Reflection
must continue to work when assembly support is disabled.

## Planned extensions

The current foundation can be extended with:

- generated metadata from annotated public headers;
- enum value catalogues;
- type-safe reflected function wrappers;
- property-inspector models and form generation;
- schema and JSON adapters that honour sensitive and transient flags;
- module-scoped unregister operations;
- filesystem watcher coordination and removal detection;
- documentation pages generated from live registry snapshots;
- an optional permission-controlled remote reflection adapter.

These extensions should build on the same registry and event topics rather
than introduce application-specific reflection implementations.
