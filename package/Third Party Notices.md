# Third Party Notices

This package contains no third-party source code or binaries. It does,
however, depend on Microsoft software that must be obtained and installed
separately by the end user.

## Kinect for Windows Runtime 2.0

`libunect.dll` links against `Kinect20.dll`, which is installed into the
Windows system directory by the Kinect for Windows Runtime 2.0. That run-
time is **not** redistributed with this package and must be installed by
the user.

- Publisher: Microsoft Corporation
- Obtain from: https://www.microsoft.com/en-us/download/details.aspx?id=44559
- Licensed under the terms accompanying that installer.

## Kinect for Windows SDK 2.0 (build dependency only)

Building this package from source requires the Kinect for Windows SDK 2.0,
which supplies `Kinect.h` and the `Kinect20.lib` import library. Neither is
redistributed here; the compiled `libunect.dll` contains no Microsoft source 
code, only an import table naming `Kinect20.dll`.

- Publisher: Microsoft Corporation
- Obtain from: https://www.microsoft.com/en-us/download/details.aspx?id=44561
- Licensed under the terms accompanying that installer.

Microsoft, Kinect, and Windows are trademarks of Microsoft Corporation.
This package is not affiliated with, endorsed by, or sponsored by
Microsoft Corporation.

## Unity

Assembly definition files and package metadata follow formats specified by
Unity Technologies. No Unity source code or binaries are included.