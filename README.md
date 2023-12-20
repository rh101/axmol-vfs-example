# axmol-vfs-example
Example of implementing virtual file system support in a project using the [Axmol](https://github.com/axmolengine/axmol) game engine.

If audio assets are to be included in compressed archives, then ensure that all audio files are stored uncompressed in the archive.  This is required in order to ensure that the file is streamed correctly by the VFS.

This implementation is only for the Windows platform, but that is only a limitation of this project, not of Axmol VFS support. To support other platforms, sub-class the relevant FileUtils implementation for that platform and implement the required methods.

This project uses [PhysFS](https://github.com/icculus/physfs) for the VFS support.

A C++ wrapper for PhysFS can also be used if required. You can find the an up-to-date version here: [physfs-cpp](https://github.com/rh101/physfs-cpp)


## Add support for VFS for a library that requires file access

There are a lot of examples in the Axmol engine code related to this. 

See these implementations in these source files:
```
[FontFreeType.cpp](https://github.com/axmolengine/axmol/blob/dev/core/2d/FontFreeType.cpp)
[AudioDecoderMp3.cpp](https://github.com/axmolengine/axmol/tree/dev/core/audio/AudioDecoderMp3.cpp)
[AudioDecoderOgg.cpp](https://github.com/axmolengine/axmol/tree/dev/core/audio/AudioDecoderOgg.cpp)
[AudioDecoderWav.cpp](https://github.com/axmolengine/axmol/tree/dev/core/audio/AudioDecoderWav.cpp)
[UserDefault.cpp](https://github.com/axmolengine/axmol/tree/dev/core/base/UserDefault.cpp)
[ZipUtils.cpp](https://github.com/axmolengine/axmol/tree/dev/core/base/ZipUtils.cpp)
```

The libraries used by the implementations above provide overridable file access routines, which would be mapped to go through `ax::FileUtils`, using the `IFileStream` interface. This ensures that any access goes through the VFS.

## Working with libraries that are difficult to use with VFS

If you are working with any library that cannot be easily adapted to work with a virtual file system, then you would need to use the real file system paths as usual (not VFS paths!). In this case, you also would not be doing any file access through `ax::FileUtils`, which would most likely be the case with or without a VFS.

An example of this would be SQLite. The following example also uses SQLiteCpp as a wrapper to simplify things even further:

```cpp
auto writablePath = ax::FileUtils::getInstance()->getNativeWritableAbsolutePath(); // Get the underlying file system path to the relevant directory
// or you can use the PHYSFS_getWriteDir() call if you have previously set the native path via PHYSFS_setWriteDir();
// auto writablePath = PHYSFS_getWriteDir();

const auto dbPath = fmt::format("{}/mydb.sqlite", writablePath); // Database file is named "mydb.sqlite", and is stored in the root of our writable path
SQLite::Database db(dbPath, SQLite::OPEN_READONLY); // Opens database

// Example query:
SQLite::Statement query(db, "SELECT * FROM MyTable WHERE Id = ?");
query.bind(1, 12345); // Id of 12345
if (query.executeStep())
{
    // Do something with the data
}
// etc. etc.
```
