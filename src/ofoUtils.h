#include <filesystem>

namespace ofo
{

namespace file {

inline std::filesystem::path resolveRelative(const std::filesystem::path& to, 
                                             const std::filesystem::path& from)
{
    using namespace std;

    if (from.empty() || to.empty()) return filesystem::path{};

    filesystem::path source = filesystem::is_directory(from) ? from : from.parent_path();

    return filesystem::absolute(source / to);
}

}

}