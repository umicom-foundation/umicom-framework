# Umicom Framework 0.5.0 — Public API Catalogue

Public headers live under `include/umicom`.  Products must not include private
Framework headers from `src`.

| Area | Public headers | Status |
|---|---|---|
| Base | `base/status.h`, `base/result.h`, `base/version.h`, `base/memory.h` | Implemented |
| Diagnostics | `diagnostics/diagnostic.h`, `diagnostics/hub.h`, `diagnostics/store.h`, `diagnostics/log.h` | Implemented |
| Messaging | `messaging/message.h`, `command_bus.h`, `query_bus.h`, `event_bus.h`, `journal.h` | Implemented foundation |
| Data | `data/data_server.h` | Implemented foundation with SQLite adapter |
| Configuration | `platform/config.h`, `platform/settings.h` | Implemented |
| Filesystem | `platform/filesystem.h` | Implemented |
| Process | `platform/process.h` | Implemented |
| Time | `platform/clock.h`, `runtime/scheduler.h` | Implemented |
| Documents and workspace | `platform/document.h`, `platform/workspace.h` | Foundation |
| Runtime | `runtime/module.h`, `module_registry.h`, `master_controller.h` | Implemented ABI 2 |
| Capabilities | `runtime/capability_registry.h`, `capability_catalogue.h` | Implemented |
| Product metadata | `runtime/application_manifest.h`, `runtime/suite.h` | Implemented |
| Security | `security/policy.h`, `security/secrets.h` | Implemented foundation |
| Toolchain | `toolchain/tool.h`, `profile.h`, `discovery.h`, `environment.h`, `dependency.h`, `build.h` | Implemented |
| Repository | `repository/repository.h`, `scaffold/repository.h` | Implemented |
| Plug-ins | `plugin/plugin.h` | Foundation |
| UI contracts | `ui/contracts.h` | Foundation |
| Aggregate include | `umicom.h` | Implemented |

## ABI rules

- Public structures that may cross module boundaries begin with a structure-size
  or ABI-version field when compatible extension is required.
- C++ classes, exceptions, STL containers, and compiler-specific RTTI never
  cross the public C ABI.
- Memory crossing an ABI boundary is allocated and released by the same owner or
  through a host-provided allocator.
- New fields are appended, not inserted into the middle of a published structure.
- Breaking changes require an ABI-version change and explicit compatibility plan.

## Ownership vocabulary

- **Borrowed**: the caller may read the object only for the documented lifetime.
- **Owned by caller**: the caller must release the object using the documented
  Framework function.
- **Transferred**: the callee assumes responsibility after a successful call.
- **Immutable snapshot**: the receiver may retain the snapshot but cannot mutate
  authoritative state through it.

## Capability maturity

- **implemented**: public contract, implementation, tests, and consumer exist.
- **foundation**: a usable bounded contract exists but requires further adapters
  or additional product proof.
- **planned**: recorded in the catalogue for architecture and scaffolding; no
  stability promise is made.
