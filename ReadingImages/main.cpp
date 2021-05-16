#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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
        files.push_back(pic.path());

    // Sort the images.
    std::sort(files.begin(), files.end());

    // Iterate the sorted files and view them.
    for (auto file : files)
    {
        std::cout << std::string(file) << std::endl;
        cv::Mat imageMatrix = cv::imread(file, cv::IMREAD_UNCHANGED);
        cv::imshow("FirstTest", imageMatrix);
        cv::waitKey(0);
    }

    return 0;
}
