#pragma once
#include "IFileStream.h"
#include "physfs.h"

namespace myapp
{
class PhysFsFileStream : public ax::IFileStream
{
public:
    PhysFsFileStream() = default;
    ~PhysFsFileStream() override;

    PhysFsFileStream(const PhysFsFileStream& other) = delete;
    PhysFsFileStream(PhysFsFileStream&& other) noexcept
        : ax::IFileStream(std::move(other)),
        _file(other._file)
    {
        other.reset();
    }

    PhysFsFileStream& operator=(const PhysFsFileStream& other) = delete;

    PhysFsFileStream& operator=(PhysFsFileStream&& other) noexcept
    {
        if (this == &other)
            return *this;
        ax::IFileStream::operator =(std::move(other));
        _file = other._file;

        other.reset();

        return *this;
    }

    bool open(std::string_view path, ax::IFileStream::Mode mode) override;
    int close() override;

    int64_t seek(int64_t offset, int origin) const override;
    int read(void* buf, unsigned int size) const override;

    int write(const void* buf, unsigned int size) const override;
    int64_t tell() const override;
    int64_t size() const override;
    bool isOpen() const override;

    //bool isReadOnly() const override;
    //int length() override;
private:
    int closeInternal();
    void reset();

    PHYSFS_File* _file = nullptr;
    //FileStream::Mode _mode;
};
}
