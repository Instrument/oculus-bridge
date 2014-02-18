Building the OculusBridge Application
=====================================


## Prerequisites

- [Cinder 0.8.5](http://libcinder.org/)
- [Oculus Rift SDK](https://developer.oculusvr.com/)


## XCode

- Open the xcode project
- Open the build settings
- Change the two user-defined values for `CINDER_PATH` and `OCULUS_SDK` to point to the respective folders on your computer.  Note: You may need to re-build the Oculus Rift libraries with `C++ Runtime Types` enabled to avoid errors when linking.


## VisualStudio 2012

- Open the solution
- Open the project properties
- Update all references to cinder and the oculus SDK to point to the respective folders on your computer