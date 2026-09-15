# Unect - Kinect v2 for Unity 3D

A modern Kinect SDK integration for the Unity 3D game engine. Provides a native interop layer for the Kinect for Windows SDK 2.0 for all supported image streams, as well as body tracking and coordinate mapping.

Supports editor and standalone player, but only runs on x64 Windows builds.

## Requirements

Besides the Kinect v2 hardware, the following software packages must be installed:

- [Kinect for Windows Runtime 2.0](https://www.microsoft.com/en-us/download/details.aspx?id=44559)
- Unity 2021.3 or newer

## Installation

1. Make sure the latest version of the Kinect for Windows Runtime 2.0 is installed. After a fresh install, reboot the system.
2. Download the [latest release](https://github.com/crud89/Unect/releases/) of this package from.
3. Import the package into Unity through "Window" -> "Package Manager" -> "+" -> "Install package from tarball..."

## Building from source

If you want to create a custom build of the package, the following software packages must be installed:

- Microsoft Visual Studio (2026 Community or later), including vcpkg, CMake and C++ development workloads.
- Kinect SDK 2.0

Development has two aspects to it. The first one is the native interop layer (`libunect.dll`). You can develop and build it directly from Visual Studio. Open the project root folder ("File" -> "Open" -> "Folder") and configure one of the build presets (`windows-msvc-x64-debug`/`windows-msvc-x64-release`). The interop layer is hosted in the `src/` directory. Do any changes you wish there and build the project as usual.

To test and use the updated interop layer from Unity, a few more steps are required. The Unity package is located in the `package/` directory of the repository. It is important that Unity maintains and updates the `.meta` files for the package. To simplify this process, the following guide sets up a soft link from a Unity project into the package directory of the repository. The workflow to setup the Unity side is as follows:

1. Open up the Developer PowerShell for Visual Studio (any other works too, but this one already sets up the environment). Navigate into the root folder of the local repository clone, e.g.: `C:\repos\unect`. Configure and build the interop layer.

	```bash
	cd C:\repos\unect
	cmake --preset windows-msvc-x64-debug && cmake --build --preset windows-msvc-x64-debug
	```

2. Create a new project in Unity (e.g. `C:\unity\unect_dev\"), open it, wait for it to generate and close Unity entirely afterwards.
3. Setup the soft link:

	```bash
    New-Item -ItemType Junction -Path "C:\unity\unect_dev\Packages\com.crud89.unect" -Target "C:\repos\unect\package"
	```

4. Synchronize the interop layer with the Unity project.

	```bash
	cmake --build --preset sync
	```

5. Open the project in Unity.

Those steps only need to be taken when setting up the project. When updating the interop layer, run a build + synchronize. By default, hot reloading is supported and Unity does not need to be closed or restarted in order to pickup the new library.