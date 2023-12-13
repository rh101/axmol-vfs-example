#include "Utility.h"
#include <vector>
#include "FileStream.h"
#include "FileUtils.h"

namespace myapp
{
bool Utility::copyFile(std::string_view src_loc, std::string_view dest_loc)
{
    auto in = ax::FileUtils::getInstance()->openFileStream(src_loc, ax::FileStream::Mode::READ);

    if (in)
    {
        const auto out = ax::FileUtils::getInstance()->openFileStream(dest_loc, ax::FileStream::Mode::WRITE);

        constexpr int BUFFER_SIZE = 16 * 1024;
        std::vector<char> buffer(BUFFER_SIZE, 0);

        while (true)
        {
            const auto count = in->read(buffer.data(), BUFFER_SIZE);
            if (count <= 0)
            {
                return false;
            }

            out->write(buffer.data(), count);

            if (count < BUFFER_SIZE)
            {
                break;
            }
        }

        return true;
    }

    return false;
}
}
