/*
 © Copyright 2023 rh101
*/

#include "PhysFsFileStream.h"
#include "axmol.h"

USING_NS_AX;

namespace myapp
{
PhysFsFileStream::~PhysFsFileStream()
{
    closeInternal();
}

bool PhysFsFileStream::open(std::string_view path, IFileStream::Mode mode)
{
    //_mode = mode;
    switch (mode)
    {
    case IFileStream::Mode::READ:
        _file = PHYSFS_openRead(std::string(path).c_str());
        break;

    case IFileStream::Mode::WRITE:
        _file = PHYSFS_openWrite(std::string(path).c_str());
        break;

    case IFileStream::Mode::APPEND:
        _file = PHYSFS_openAppend(std::string(path).c_str());
        break;

    //case IFileStream::Mode::OVERLAPPED:
    //    _file = PHYSFS_openReadWrite(std::string(path).c_str()); // Custom version of PhysFS, not available here
    //    break;

    case IFileStream::Mode::OVERLAPPED:
        _file = PHYSFS_openWrite(std::string(path).c_str());
        break;

    default:
        AXASSERT(false, "UNKNOWN FileStream::Mode");
        break;
    }

    return _file != nullptr;
}

int PhysFsFileStream::close()
{
    return closeInternal();
}

int64_t PhysFsFileStream::seek(int64_t offset, int origin) const
{
    int64_t result;
    switch (origin)
    {
    case SEEK_SET:
        result = offset;
        if (PHYSFS_seek(_file, result) == 0)
            return -1;

        return result;

    case SEEK_CUR:
        result = PHYSFS_tell(_file) + offset;
        if (PHYSFS_seek(_file, result) == 0)
            return -1;

        return result;

    case SEEK_END:
        result = PHYSFS_fileLength(_file) + offset;
        if (PHYSFS_seek(_file, result) == 0)
            return -1;

        return result;
    default:;
    }

    return -1; // -1 = error, > 0 stream position = success
}

int PhysFsFileStream::closeInternal()
{
    if (!_file)
    {
        return -1;
    }

    const auto result = PHYSFS_close(_file);
    if (result == 0)
    {
        return -1; // error
    }

    _file = nullptr;
    return 0; // successful
}

int PhysFsFileStream::read(void* buf, unsigned int size) const
{
    return PHYSFS_readBytes(_file, buf, size);
}

int PhysFsFileStream::write(const void* buf, unsigned int size) const
{
    return PHYSFS_writeBytes(_file, buf, size);
}

int64_t PhysFsFileStream::tell() const
{
    return PHYSFS_tell(_file);
}

int64_t PhysFsFileStream::size() const
{
    return PHYSFS_fileLength(_file);
}

bool PhysFsFileStream::isOpen() const
{
    return _file != nullptr;
}

void PhysFsFileStream::reset()
{
    _file = nullptr;
}

}