/*
 © Copyright 2023 rh101
*/

#include "VfsFileUtils.h"
//#include "physfs.hpp" // If using PhysFS-cpp from https://github.com/kahowell/physfs-cpp
#include "physfs.h"
#include "PhysFsFileStream.h"
#include "Utility.h"
#include "fmt/format.h"

USING_NS_AX;
namespace myapp
{

VfsFileUtils* VfsFileUtils::create()
{
    auto* fileUtils = new (std::nothrow) VfsFileUtils();
    fileUtils->init();
    return fileUtils;
}

VfsFileUtils::VfsFileUtils()
{
}

VfsFileUtils::~VfsFileUtils()
{
    PHYSFS_deinit();
}

bool VfsFileUtils::init()
{
    if (!FileUtilsWin32::init())
    {
        return false;
    }
    setSearchPaths({});

    const auto appDataPathWritePath = getNativeWritableAbsolutePath();
    setWritablePath(appDataPathWritePath);

    const auto defaultResourceDir = _defaultResRootPath;

    PHYSFS_init(nullptr);

    setDefaultResourceRootPath("");

    // Set up the write directory for this platform
    PHYSFS_setWriteDir(appDataPathWritePath.c_str());
    // add it to be first in search paths
    PHYSFS_mount(appDataPathWritePath.c_str(), "", false);

    /* then add the default resource directory to the search path */
    PHYSFS_mount(defaultResourceDir.c_str(), "", true);

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    // Add compiled shaders path
    auto axslcDir = fmt::format("{}{}", getAppRoot(), "axslc"sv);
    PHYSFS_mount(axslcDir.c_str(), "", true);
#else
    // Add compiled shaders path
    PHYSFS_mount("axslc", "", true);
#endif

    return true;
}

std::string VfsFileUtils::getWritablePath() const
{
    return "/"; // The base path will be our writable path
}

bool VfsFileUtils::isAbsolutePath(std::string_view strPath) const
{
    return ax::FileUtilsWin32::isAbsolutePath(strPath);
    //return (strPath[0] == '/') /*|| FileUtilsWin32::isAbsolutePath(strPath)*/;  // We should never ever encounter a path that is for real file system!
}

int64_t VfsFileUtils::getFileSize(std::string_view filepath) const
{
    const auto file = PHYSFS_openRead(filepath.data());

    if (!file)
        return 0;

    const auto size = PHYSFS_fileLength(file);
    PHYSFS_close(file);

    return size;
}

std::vector<std::string> VfsFileUtils::listFiles(std::string_view dirPath) const
{
    std::vector<std::string> files;
    char** listBegin = PHYSFS_enumerateFiles(std::string(dirPath).c_str()); // to ensure it's null terminated, cast it to std::string first
    for (char** file = listBegin; *file != nullptr; file++) {
        files.push_back(*file);
    }
    PHYSFS_freeList(listBegin);
    return files;
}

void VfsFileUtils::listFilesRecursively(std::string_view dirPath, std::vector<std::string>* files) const
{
    if (!files)
    {
        return;
    }

    PHYSFS_enumerate(std::string(dirPath).c_str(),
        [](void* data, const char* origdir, const char* fname) -> PHYSFS_EnumerateCallbackResult {
            auto* list = static_cast<std::vector<std::string>*>(data);
            list->emplace_back(fname);
            return PHYSFS_EnumerateCallbackResult::PHYSFS_ENUM_OK;
        }, files);
}

void VfsFileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
    FileUtilsWin32::setSearchPaths(searchPaths);
}

bool VfsFileUtils::writeStringToFile(std::string_view dataStr, std::string_view fullPath) const
{
    auto out = openFileStream(fullPath, FileStream::Mode::WRITE);

    const auto totalSize = dataStr.size();
    const auto result = out->write(dataStr.data(), dataStr.size()) == totalSize;

    return result;
}

bool VfsFileUtils::writeDataToFile(const ax::Data& data, std::string_view fullPath) const
{
    auto out = openFileStream(fullPath, FileStream::Mode::WRITE);

    const auto totalSize = data.getSize();
    const auto result = out->write(data.getBytes(), data.getSize()) == totalSize;
    return result;
}

std::unique_ptr<ax::IFileStream> VfsFileUtils::openFileStream(std::string_view filePath, FileStream::Mode mode) const
{
    PhysFsFileStream fs;

    if (fs.open(filePath, mode))
    {
        return std::make_unique<PhysFsFileStream>(std::move(fs));
    }

    return nullptr;
}

bool VfsFileUtils::isFileExistInternal(std::string_view strFilePath) const
{
    if (strFilePath.empty())
    {
        return false;
    }

    return PHYSFS_exists(strFilePath.data());
}

bool VfsFileUtils::renameFile(std::string_view oldfullpath, std::string_view newfullpath) const
{
    AXASSERT(!oldfullpath.empty(), "Invalid path");
    AXASSERT(!newfullpath.empty(), "Invalid path");

    if (FileUtils::getInstance()->isFileExist(newfullpath))
    {
        if (!removeFile(newfullpath))
        {
            AXLOGERROR("Fail to delete file %s !Error code is 0x%x", newfullpath.data(), GetLastError());
        }
    }

    if (Utility::copyFile(oldfullpath, newfullpath))
    {
        return removeFile(oldfullpath);
    }
    else
    {
        AXLOGERROR("Fail to rename file %s to %s !Error code is 0x%x", oldfullpath.data(), newfullpath.data(), GetLastError());
        return false;
    }
}

bool VfsFileUtils::isDirectoryExistInternal(std::string_view dirPath) const
{
#if !defined(_NDEBUG)
    if (isAbsolutePath(dirPath))
    {
        if (dirPath.find(':') != std::string::npos)
            bool a = true;
    }
#endif
    return PHYSFS_exists(dirPath.data());
}

bool VfsFileUtils::removeFile(std::string_view filepath) const
{
    return PHYSFS_delete(filepath.data());
}

bool VfsFileUtils::createDirectory(std::string_view dirPath) const
{
    return PHYSFS_mkdir(dirPath.data());
}

bool VfsFileUtils::removeDirectory(std::string_view dirPath) const
{
    return PHYSFS_delete(dirPath.data());
}

ax::FileUtils::Status VfsFileUtils::getContents(std::string_view filename, ax::ResizableBuffer* buffer) const
{
    if (filename.empty())
        return FileUtils::Status::NotExists;

    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    if (fullPath.empty())
        return FileUtils::Status::NotExists;

    auto file = PHYSFS_openRead(fullPath.c_str());

    if (!file)
        return FileUtils::Status::OpenFailed;

    const auto size = PHYSFS_fileLength(file);
    if (size <= 0)
    {
        return FileUtils::Status::OK;
    }

    if (size > ULONG_MAX)
    {
        return FileUtils::Status::TooLarge;
    }

    buffer->resize(size);

    const auto sizeRead = PHYSFS_readBytes(file, buffer->buffer(), size);
    if (sizeRead < size) {
        AXLOG("Get data from file(%s) failed, error code is %s", fullPath.data(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        buffer->resize(sizeRead);
        return FileUtils::Status::ReadFailed;
    }

    PHYSFS_close(file);

    return FileUtils::Status::OK;
}
}