/*
 © Copyright 2023 rh101
*/

#include "VfsFileUtils.h"
//#include "physfs.hpp" // If using PhysFS-cpp from https://github.com/kahowell/physfs-cpp
#include "physfs.h"
#include "PhysFsFileStream.h"
#include "Utility.h"
#include "fmt/format.h"
#include "platform/android/jni/JniHelper.h"
#include "platform/android/jni/Java_dev_axmol_lib_AxmolEngine.h"
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
    if (!FileUtilsAndroid::init())
    {
        return false;
    }
    setSearchPaths({});

    const auto appDataPathWritePath = getNativeWritableAbsolutePath();
    setWritablePath(appDataPathWritePath);

    const auto defaultResourceDir = _defaultResRootPath;

    PHYSFS_AndroidInit androidInit;
    androidInit.context = ax::JniHelper::getActivity();
    androidInit.jnienv = ax::JniHelper::getEnv();
    std::string apkPath(getApkPath());

    PHYSFS_init((const char*)&androidInit);

    setDefaultResourceRootPath("");

    // Set up the write directory for this platform
    PHYSFS_setWriteDir(appDataPathWritePath.c_str());
    // add it to be first in search paths
    PHYSFS_mount(appDataPathWritePath.c_str(), "", false);

    PHYSFS_mount(PHYSFS_getBaseDir(), "", true);

    PHYSFS_setRoot(PHYSFS_getBaseDir(), "/assets");

    // Add compiled shaders path
    PHYSFS_mount("axslc", "", true);

    return true;
}

std::string VfsFileUtils::getWritablePath() const
{
    return "/"; // The base path will be our writable path
}

bool VfsFileUtils::isAbsolutePath(std::string_view strPath) const
{
    return (strPath[0] == '/');
    //return FileUtilsAndroid::isAbsolutePath(strPath);
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
void VfsFileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
    FileUtilsAndroid::setSearchPaths(searchPaths);
    //bool existDefaultRootPath = false;
    //_originalSearchPaths = searchPaths;

    //_fullPathCache.clear();
    //_searchPathArray.clear();

    //for (const auto& path : _originalSearchPaths)
    //{
    //    std::string fullPath;

    //    fullPath = path;
    //    if (!path.empty() && path[path.length() - 1] != '/')
    //    {
    //        fullPath += "/";
    //    }

    //    if (!existDefaultRootPath && path == _defaultResRootPath)
    //    {
    //        existDefaultRootPath = true;
    //    }
    //    _searchPathArray.push_back(fullPath);
    //}

    //if (!existDefaultRootPath)
    //{
    //    //CCLOG("Default root path doesn't exist, adding it.");
    //    _searchPathArray.push_back(_defaultResRootPath);
    //}
}
bool VfsFileUtils::writeStringToFile(std::string_view dataStr, std::string_view fullPath) const
{
    auto out = std::unique_ptr<IFileStream>(FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::WRITE));

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

std::unique_ptr<ax::IFileStream> VfsFileUtils::openFileStream(std::string_view filePath, IFileStream::Mode mode) const
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

bool VfsFileUtils::renameFile(std::string_view path, std::string_view oldname, std::string_view name) const
{
    return FileUtilsAndroid::renameFile(path, oldname, name);
}

bool VfsFileUtils::renameFile(std::string_view oldfullpath, std::string_view newfullpath) const
{
    AXASSERT(!oldfullpath.empty(), "Invalid path");
    AXASSERT(!newfullpath.empty(), "Invalid path");

    if (FileUtils::getInstance()->isFileExist(newfullpath))
    {
        if (!removeFile(newfullpath))
        {
            AXLOGE("Fail to delete file {}", newfullpath.data());
        }
    }

    if (Utility::copyFile(oldfullpath, newfullpath))
    {
        return removeFile(oldfullpath);
    }
    else
    {
        AXLOGE("Fail to rename file {} to {}!", oldfullpath.data(), newfullpath.data());
        return false;
    }
}

bool VfsFileUtils::isDirectoryExistInternal(std::string_view dirPath) const
{
    return PHYSFS_exists(dirPath.data());
}

bool VfsFileUtils::removeFile(std::string_view filepath) const
{
    return PHYSFS_delete(filepath.data());
}

bool VfsFileUtils::createDirectories(std::string_view dirPath) const
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
        AXLOGD("Get data from file({}) failed, error code is {}", fullPath, (int)PHYSFS_getLastErrorCode());
        buffer->resize(sizeRead);
        return FileUtils::Status::ReadFailed;
    }

    PHYSFS_close(file);

    return FileUtils::Status::OK;
}

std::vector<std::string> VfsFileUtils::listFiles(std::string_view dirPath) const
{
    std::vector<std::string> files;
    char** listBegin = PHYSFS_enumerateFiles(std::string(dirPath).c_str());
    for (char** file = listBegin; *file != NULL; file++) {
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

    PHYSFS_enumerate(dirPath.data(),
                           [](void* data, const char* origdir, const char* fname) -> PHYSFS_EnumerateCallbackResult {
                               auto* list = static_cast<std::vector<std::string>*>(data);
                               list->emplace_back(fname);
                               return PHYSFS_EnumerateCallbackResult::PHYSFS_ENUM_OK;
                           }, files);
}
}
