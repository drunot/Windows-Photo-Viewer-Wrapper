# Windows Photo Viewer Wrapper

An executeble for opening Windows Photo Viewer.

The Windows Photo Viewer DLLs is requered for this program to work.

## Register and unregister the app as a possible default image viewer
To register it, press left shift while opening the app or in a command promt with admin rights write:
``` shell
$ "Path\to\Windows Photo Viewer.exe" -register
```
To unregister it, press right shift while opening the app or in a command promt with admin rights write:
``` shell
$ "Path\to\Windows Photo Viewer.exe" -unregister
```