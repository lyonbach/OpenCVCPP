#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <iostream>
#include <string>


int main(int argc, char const *argv[])
{
    
    if (argc != 2)
    {
        std::cout << "Exactly one argument required!" << std::endl;
        return 1;
    }

    std::string videoPath = std::string(argv[1]);
    std::cout << "Opening video:\n    " << videoPath << std::endl;

    cv::VideoCapture videoCapture(videoPath);
    cv::Mat frame;

    std::string screenTitle = std::filesystem::path(videoPath).filename();
    while ( true )
    {
        videoCapture.read(frame);
        if (frame.empty())
            return 0;

        cv::imshow(screenTitle, frame);
        int key = cv::waitKey(100);
        if (key == 27)
            break;
    }

    return 0;
}
