# ttddbg - Time Travel Debugging IDA plugin

This plugin adds a new debugger to IDA which supports loading [Time Travel Debugging](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/time-travel-debugging-overview) traces generated using WinDBG Preview.

![ttddbg main window](screenshots/main_window.png)

This plugin supports both x86 and x64 traces, and by extension IDA and IDA64.

## Installation

Installing the plugin can be done using the installer from the [releases page]([releases](https://github.com/airbus-cert/ttddbg/releases)). The installer will automatically install the required dependencies, provided you have a copy of [WinDBG Preview](https://apps.microsoft.com/store/detail/windbg-preview/9PGJGD53TN86) installed.

## Usage

Once installed, you can use the plugin by selecting the `ttddbg` debugger in the IDA interface, and specifying your `*.run` file as the "Application". For help on generating a `.run` file, see [`HOWTO_TIME_TRAVEL.md`](HOWTO_TIME_TRAVEL.md).

![ttddbg debugger](screenshots/ida_ttddbg_dropdown.png)

![ttdbg debugger setup](screenshots/ida_ttddbg_run.png)

| Icon | Action |
|------|--------|
|![backward_icon](screenshots/ida_backward_icon.png) | Go to previous breakpoint |
|![single_step_icon](screenshots/ida_single_step_icon.png) | Single step backward (RIP - one instruction) |
|![timeline_icon](screenshots/ida_timeline_icon.png) | Manage the timeline of interesting events (Threads Created/Terminated, Module Loaded/Unloaded, Exceptions, Custom) |

## Building the project

Prerequisites:

- A copy of the IDA SDK (available from the [download center](https://hex-rays.com/download-center/) using your IDA Pro credentials)
- A copy of `TTDReplay.dll` (usually in `C:\Program Files\WindowsApps\[WinDBG folder]\amd64\ttd\`)
- A copy of `TTDReplayCPU.dll` (usually in `C:\Program Files\WindowsApps\[WinDBG folder]\amd64\ttd\`)

And let CMAKE do its magic!

```console
$ git clone git@github.com:airbus-cert/ttddbg.git --recursive
$ mkdir build
$ cd build
$ cmake ..\ttddbg -DIDA_SDK_SOURCE_DIR=[PATH_TO_IDA_SDK_ROOT_FOLDER] -DCPACK_PACKAGE_INSTALL_DIRECTORY="IDA Pro 7.7"
$ cmake --build . --target package --config release
```

## Developer corner

To create a dev solution:

```console
$ git clone git@github.com:airbus-cert/ttddbg.git --recursive
$ mkdir build
$ cd build
$ cmake ..\ttddbg -DIDA_SDK_SOURCE_DIR=[PATH_TO_IDA_SDK_ROOT_FOLDER] -DBUILD_TESTS=ON
```

# Credits and references

Greetz to [commial](https://github.com/commial) for his work on [ttd-bindings](https://github.com/commial/ttd-bindings)!
