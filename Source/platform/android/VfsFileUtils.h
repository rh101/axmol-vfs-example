#pragma once

#include "win32/FileUtils-win32.h"
#include "axmol.h"
#include <string>

namespace myapp
{
class AX_DLL VfsFileUtils : public ax::FileUtilsAndroid
{
    friend class FileUtils;

public:
    static VfsFileUtils* create();

    VfsFileUtils();

    virtual ~VfsFileUtils();

    /* override functions */
    bool init() override;

    std::string getWritablePath() const override;

    bool isAbsolutePath(std::string_view strPath) const override;
    int64_t getFileSize(std::string_view filepath) const override;
    void setSearchPaths(const std::vector<std::string>& searchPaths) override;
    bool writeStringToFile(std::string_view dataStr, std::string_view fullPath) const override;
    bool writeDataToFile(const ax::Data& data, std::string_view fullPath) const override;

    std::unique_ptr<ax::IFileStream> openFileStream(std::string_view filePath, ax::IFileStream::Mode mode) const override;

    bool isFileExistInternal(std::string_view strFilePath) const override;

    /**
    *  Renames a file under the given directory.
    *
    *  @param path     The parent directory path of the file, it must be an absolute path.
    *  @param oldname  The current name of the file.
    *  @param name     The new name of the file.
    *  @return True if the file have been renamed successfully, false if not.
    */
    bool renameFile(std::string_view path,std::string_view oldname, std::string_view name) const override;

    /**
    *  Renames a file under the given directory.
    *
    *  @param oldfullpath  The current path + name of the file.
    *  @param newfullpath  The new path + name of the file.
    *  @return True if the file have been renamed successfully, false if not.
    */
    bool renameFile(std::string_view oldfullpath, std::string_view newfullpath) const override;

    /**
    *  Checks whether a directory exists without considering search paths and resolution orders.
    *  @param dirPath The directory (with absolute path) to look up for
    *  @return Returns true if the directory found at the given absolute path, otherwise returns false
    */
    bool isDirectoryExistInternal(std::string_view dirPath) const override;

    /**
    *  Removes a file.
    *
    *  @param filepath The full path of the file, it must be an absolute path.
    *  @return True if the file have been removed successfully, false if not.
    */
    bool removeFile(std::string_view filepath) const override;

    /**
    *  Creates a directory.
    *
    *  @param dirPath The path of the directory, it must be an absolute path.
    *  @return True if the directory have been created successfully, false if not.
    */
    bool createDirectories(std::string_view dirPath) const override;

    /**
    *  Removes a directory.
    *
    *  @param dirPath  The full path of the directory, it must be an absolute path.
    *  @return True if the directory have been removed successfully, false if not.
    */
    bool removeDirectory(std::string_view dirPath) const override;


    FileUtils::Status getContents(std::string_view filename, ax::ResizableBuffer* buffer) const override;

    /**
     *  Gets full path for filename, resolution directory and search path.
     *
     *  @param filename The file name.
     *  @param resolutionDirectory The resolution directory.
     *  @param searchPath The search path.
     *  @return The full path of the file. It will return an empty string if the full path of the file doesn't exist.
     */
    //std::string getPathForFilename(std::string_view filename, std::string_view resolutionDirectory, std::string_view searchPath) const override;

    /**
     *  Gets full path for the directory and the filename.
     *
     *  @note Only iOS and Mac need to override this method since they are using
     *        `[[NSBundle mainBundle] pathForResource: ofType: inDirectory:]` to make a full path.
     *        Other platforms will use the default implementation of this method.
     *  @param directory The directory contains the file we are looking for.
     *  @param filename  The name of the file.
     *  @return The full path of the file, if the file can't be found, it will return an empty string.
     */
    //std::string getFullPathForDirectoryAndFilename(std::string_view directory, std::string_view filename) const override;

    std::vector<std::string> listFiles(std::string_view dirPath) const override;
    void listFilesRecursively(std::string_view dirPath, std::vector<std::string>* files) const override;
};
}
