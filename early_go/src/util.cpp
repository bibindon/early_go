#include "util.hpp"

#include "stdafx.hpp"

using std::vector;
using std::string;

namespace early_go
{
vector<char> util::get_resource(const string& query)
{
    vector<char> ret;
    sqlite3* db { nullptr };
    if (sqlite3_open(constants::DATABASE_NAME.c_str(), &db) != SQLITE_OK)
    {
        sqlite3_close(db);
        THROW_WITH_TRACE("Failed to open a database.");
    }
    sqlite3_stmt *_statement = nullptr;
    sqlite3_prepare_v2(db, query.c_str(), -1, &_statement, nullptr);
    bool is_found{false};
    while (sqlite3_step(_statement) == SQLITE_ROW)
    {
        if (!is_found)
        {
            is_found = true;
        }
        else
        {
            sqlite3_finalize(_statement);
            sqlite3_close(db);
            THROW_WITH_TRACE("There are multiple specified resources.\n query: " + query);
        }
        const char *blob = (char *)sqlite3_column_blob(_statement, 0);
        int data_count = sqlite3_column_bytes(_statement, 0);
        ret.reserve(data_count);
        ret.insert(ret.begin(), blob, blob + data_count);
    }
    sqlite3_finalize(_statement);
    sqlite3_close(db);
    if (!is_found)
    {
        THROW_WITH_TRACE("Failed to find a resource.\n query: " + query);
    }
    return ret;
}
vector<char> util::get_model_resource(const string& model_name)
{
    vector<char> buffer =
        get_resource("SELECT DATA FROM MODEL WHERE FILENAME = '" + model_name + "';");
    return buffer;
}
vector<char> util::get_model_texture_resource(const string& model_name, const string& texture_name)
{
    string query{};
    query = "SELECT DATA FROM MODEL WHERE FILENAME = '";
    query += model_name;
    query = query.erase(query.find_last_of('/') + 1);
    query += texture_name;
    query += "';";

    vector<char> buffer = get_resource(query);
    return buffer;
}
vector<char> util::get_shader_resource(const string& shader_name)
{
    vector<char> buffer =
        get_resource("SELECT DATA FROM SHADER WHERE FILENAME = 'shader/" + shader_name + "';");
    return buffer;
}
vector<char> util::get_image_resource(const string& image_name)
{
    vector<char> buffer =
        get_resource("SELECT DATA FROM IMAGE WHERE FILENAME = '" + image_name + "';");
    return buffer;
}
vector<char> util::get_lua_resource(const string& filename)
{
// Read from local file.
#if defined(DEBUG) || defined(_DEBUG)
    string filepath = "./res/" + filename;
    std::ifstream ifs(filepath);
    string work((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    vector<char> buffer(work.begin(), work.end());
// Read from database.
#else
    vector<char> buffer =
        get_resource("SELECT DATA FROM SCRIPT WHERE FILENAME = '" + filename + "';");
#endif
    return buffer;
}
vector<char> util::get_font_resource(const string& resname)
{
    vector<char> buffer =
        get_resource("SELECT DATA FROM FONT WHERE FILENAME = '" + resname + "';");
    return buffer;
}
}
