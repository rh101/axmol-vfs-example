# axmol-vfs-example
Example of implementing virtual file system support in a project using the [Axmol](https://github.com/axmolengine/axmol) game engine.

If audio assets are to be included in compressed archives, then ensure that all audio files are stored uncompressed in the archive.  This is required in order to ensure that the file is streamed correctly by the VFS.

This implementation is only for the Windows platform, but that is only a limitation of this project, not of Axmol VFS support. To support other platforms, sub-class the relevant FileUtils implementation for that platform and implement the required methods.

This project uses [PhysFS](https://github.com/icculus/physfs) for the VFS support.

A C++ wrapper for PhysFS can also be used if required. You can find the an up-to-date version here: [physfs-cpp](https://github.com/rh101/physfs-cpp)
