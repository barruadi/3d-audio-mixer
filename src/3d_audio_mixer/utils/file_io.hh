#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace nutils
{
    class FileIO
    {
        public:
            static bool read_json(const std::string& filename, nlohmann::json& out_json)
            {
                std::ifstream file(filename);
                if (!file.is_open())
                {
                    return false;
                }

                try
                {
                    file >> out_json;
                }
                catch (const std::exception& e)
                {
                    return false;
                }

                return true;
            }

            static bool write_json(const std::string& filename, const nlohmann::json& json)
            {

            }
    };
} // namespace nutils
