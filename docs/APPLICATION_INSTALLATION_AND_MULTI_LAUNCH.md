<!--
Umicom Framework
File: docs/APPLICATION_INSTALLATION_AND_MULTI_LAUNCH.md
Author: Sammy Hegab, Umicom Foundation
Licence: MIT
-->

# Application Installation and Multi-Launch

This Framework capability supplies two small models that can be rendered by
any supported user interface.

`UmiInstallerSelection` represents the applications offered by an installer.
It records which products are available, required, selected by default and how
much disk space they are expected to use. Required products cannot be cleared.

`UmiApplicationLaunchSelection` projects the live application runtime
catalogue into a list of products that can be opened. It remembers user choices
while runtime state is refreshed. Its execute operation sends every selected
product through `UmiApplicationLauncher`; it never starts a process directly.

The multi-launch report contains one result per selected application. The
caller can therefore explain a partial result clearly, such as two products
starting successfully while one product could not be opened.

Umicom Desk owns the user-facing composition through `UmiDeskRuntime`. GTK4 and
future adapters render the same copied choices and send semantic select or
launch actions back to that runtime.

## Adding a product

Keep the product repository thin:

1. Declare its identity and executable in its application manifest.
2. Let the suite composition assign an installer component.
3. Upsert its installed presence into the Framework runtime catalogue.
4. Render product-specific panels, but reuse the Framework launcher and process
   supervisor.

This separation means a future application receives installation and
multi-launch behaviour without copying code from Studio, Trader, Bank or TMS.

