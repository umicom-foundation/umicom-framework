# Supported features

This console application demonstrates how an application module consumes the
Framework SDK. It reads a selected application manifest and displays the
application name, build option and installer component. It does not change Git
history, install software or execute another application.

# Getting started

1. Build and install the Framework SDK using the supported configuration for
   your computer.
2. Read the Framework guide <code>docs/guides/application-module-discovery.html</code>.
   It explains standalone builds and registration in an Applications suite.
3. Pass an absolute path to <code>application.umicom.yaml</code> to the built
   <code>umicom-module-example</code> executable. Expect the option
   <code>UMICOM_APPLICATIONS_BUILD_MODULE_EXAMPLE</code>.
4. Extend <code>main.c</code> by calling Framework services for your application's
   work. Keep reusable services in Framework, and keep this repository focused
   on its application interface and composition.

# Limitations

The example is a console program. It has no GTK frontend and does not add an
experience to the built-in product catalogue. A new graphical application must
supply its own frontend or implement and qualify a Framework product experience.
Discovery registers build and installer rules; it does not certify those rules
or the resulting application.
