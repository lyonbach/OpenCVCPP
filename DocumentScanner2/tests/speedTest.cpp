#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int testSize = 1000;

void copyTestImages(fs::path &sourceFolder, fs::path &targetFolder)
{
    int counter = 0;
    while (true)
    {
        for (fs::path file : fs::directory_iterator(sourceFolder))
        {
            if (!file.has_extension()) continue;
            
            std::string ext = file.extension();
            
            fs::path targetFile(targetFolder / ((std::string)file.filename().replace_extension("") + '_' + std::to_string(counter) + ext));
            fs::copy_file(file, targetFile);
            counter++;
            if (counter >= testSize) break;
        }
        if (counter >= testSize) break;
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Exactly one argument required!" << std::endl;
        return 1;
    }

    std::cout << "Test count: " << testSize << std::endl;

    fs::path testDir = (std::string)argv[1];
    fs::path tempDir = (std::string)argv[1] + "/temp";

    std::cout << "Creating temporary directory as:\n\t" << tempDir << std::endl;

    fs::create_directory(tempDir);
    copyTestImages(testDir, tempDir);

    // fs::remove_all(tempDir);

    return 0;
}
