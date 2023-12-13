#pragma once
#include <string_view>

namespace myapp
{
class Utility
{
public:
	static bool copyFile(std::string_view src_loc, std::string_view dest_loc);
};

}
