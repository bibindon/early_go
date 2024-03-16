#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <string>

namespace early_go
{
class util
{
public:
    static std::vector<char> get_resource(const std::string& query);
    static std::vector<char> get_model_resource(const std::string& model_name);
    static std::vector<char> get_model_texture_resource(
        const std::string& model_name, const std::string& texture_name);
    static std::vector<char> get_shader_resource(const std::string& shader_name);
    static std::vector<char> get_image_resource(const std::string& image_name);
};
}
#endif

