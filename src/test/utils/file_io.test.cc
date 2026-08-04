#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>

#include "utils/file_io.hh"

static const std::string kTmpPath = "/tmp/3dam_file_io_test.json";

class FileIO_ReadJson : public ::testing::Test
{
protected:
    void TearDown() override { std::remove(kTmpPath.c_str()); }
};

TEST_F(FileIO_ReadJson, ReturnsFalseForMissingFile)
{
    nlohmann::json out;
    EXPECT_FALSE(nutils::FileIO::read_json("/tmp/does_not_exist_3dam.json", out));
}

TEST_F(FileIO_ReadJson, ReturnsTrueAndParsesValidJson)
{
    std::ofstream f(kTmpPath);
    f << R"({"key": 42})";
    f.close();

    nlohmann::json out;
    ASSERT_TRUE(nutils::FileIO::read_json(kTmpPath, out));
    EXPECT_EQ(out["key"].get<int>(), 42);
}

TEST_F(FileIO_ReadJson, ReturnsFalseForInvalidJson)
{
    std::ofstream f(kTmpPath);
    f << "{ not valid json !!!";
    f.close();

    nlohmann::json out;
    EXPECT_FALSE(nutils::FileIO::read_json(kTmpPath, out));
}

class FileIO_WriteJson : public ::testing::Test
{
protected:
    void TearDown() override { std::remove(kTmpPath.c_str()); }
};

TEST_F(FileIO_WriteJson, ReturnsTrueAndFileExists)
{
    nlohmann::json data = {{"x", 1}, {"y", 2}};
    ASSERT_TRUE(nutils::FileIO::write_json(kTmpPath, data));

    std::ifstream f(kTmpPath);
    EXPECT_TRUE(f.is_open());
}

TEST_F(FileIO_WriteJson, RoundtripPreservesData)
{
    nlohmann::json written = {
        {"name", "test"},
        {"value", 3.14f},
        {"active", true}
    };
    ASSERT_TRUE(nutils::FileIO::write_json(kTmpPath, written));

    nlohmann::json read;
    ASSERT_TRUE(nutils::FileIO::read_json(kTmpPath, read));

    EXPECT_EQ(read["name"].get<std::string>(), "test");
    EXPECT_NEAR(read["value"].get<float>(), 3.14f, 1e-4f);
    EXPECT_EQ(read["active"].get<bool>(), true);
}

TEST_F(FileIO_WriteJson, UsesFourSpaceIndentation)
{
    nlohmann::json data = {{"key", "val"}};
    ASSERT_TRUE(nutils::FileIO::write_json(kTmpPath, data));

    std::ifstream f(kTmpPath);
    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());

    // 4-space indent means "    " before "key"
    EXPECT_NE(content.find("    \"key\""), std::string::npos);
}
