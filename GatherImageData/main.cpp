#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

std::vector<std::string> split(std::string strIn)
{
    std::vector<std::string> splitted;
    std::stringstream stream(strIn);
    std::string strInter;
    while(std::getline(stream, strInter, '_'))
    {
        splitted.push_back(strInter);
    }
    return splitted;
}


int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Exactly one argument required!" << std::endl;
        return 1;
    }

    std::string picturesDir = std::string(argv[1]);
    std::vector<std::filesystem::path> files;

    // List all the files in the given directory and store them in a vector.
    for (const auto & pic : std::filesystem::directory_iterator(picturesDir))
        files.push_back(pic);

    // Sort the images.
    std::sort(files.begin(), files.end());

    std::string dataRow;

    std::vector <std::string> splitted;
    std::ofstream outFile;
    outFile.open("./test.log");
    for (auto file : files)
    {
        std::vector splitted = split(file.filename().replace_extension(""));
        if (!splitted.size())
            continue;
        std::string suffix = splitted.at(1);

        int filesize = std::filesystem::file_size(file);
        outFile << suffix + ", " + std::to_string(filesize) + '\n';
    }

    outFile.close();
    return 0;
}
