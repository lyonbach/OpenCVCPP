#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>

struct ContourData
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
};

ContourData getContours(cv::Mat *preProcessed)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(*preProcessed, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    ContourData result({contours, hierarchy});
    return result;
}

void preProcess(cv::Mat *source, cv::Mat *destination)
{

    cv::Mat imageDataGray, imageDataBlur, imageDataCanny, kernel;

    // Convert image to gray scale.
    cv::cvtColor(*source, imageDataGray, cv::COLOR_RGB2GRAY);


    // Blur some.
    cv::GaussianBlur(imageDataGray, imageDataBlur, cv::Size(7, 7), 3, 0);
    cv::imshow("gray", imageDataBlur);
    cv::waitKey(0);

    // Apply canny.
    cv::Canny(imageDataBlur, imageDataCanny, 25, 50);

    // Apply dilation.
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::dilate(imageDataCanny, *destination, kernel);

}


int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Exactly one argument required!" << std::endl;
        return 1;
    }

    std::string imagePath = std::string(argv[1]);

    cv::Mat imageData, preProcessed;
    cv::Mat kernel;

    // Definitions for contour detection.
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    // Read image data.
    imageData = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    preProcess(&imageData, &preProcessed);
    ContourData result = getContours(&preProcessed);
    cv::drawContours(imageData, result.contours, -1, cv::Scalar(255, 0, 255), 2);

    cv::imshow("Raw", imageData);
    // cv::imshow("Dilate Image", preProcessed);
    cv::waitKey(0);



    return 0;
}

