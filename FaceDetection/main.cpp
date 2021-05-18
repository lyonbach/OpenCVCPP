#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

#define WINDOW_TITLE "Face Detection"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cout << "Exactly two arguments required!" << std::endl;
        return 1;
    }

    std::string classifierSourceFilePath = std::string(argv[2]);
    cv::CascadeClassifier classifier;
    classifier.load(classifierSourceFilePath);
    if(classifier.empty())
    {
        std::cout << "Unable to load classifier source file!" << std::endl;
        return 1;
    }

    // Necessary definitions
    cv::Mat imageData, scaledImageData, scaledImageDataGray;
    // Load image data 
    std::string imagePath = std::string(argv[1]);
    imageData = cv::imread(imagePath, cv::IMREAD_UNCHANGED);

    // Scale image to the desired size
    int targetHeight = 600;
    float coefficient = 600.0f / imageData.rows;
     cv::resize(imageData, scaledImageData, cv::Size(targetHeight, (int)(imageData.cols * coefficient)));

    // Convert to grayscale for face detection
    cv::cvtColor(scaledImageData, scaledImageDataGray, cv::COLOR_BGR2GRAY);

    // Show the original image
    cv::imshow(WINDOW_TITLE, scaledImageData);
    cv::waitKey(0);

    // Detect faces
    std::vector<cv::Rect> faceBoundingRects;
    classifier.detectMultiScale(scaledImageDataGray, faceBoundingRects, 1.1, 10, 0, cv::Size(50, 50));
    if (faceBoundingRects.empty())
    {
        std::cout << "Unable to detect any faces!" << std::endl;
        return 0;
    }
    else 
    {
        std::cout << "Detected " << faceBoundingRects.size() << " face(s)..." << std::endl;
    }

    for (cv::Rect rect : faceBoundingRects)
    {
        cv::rectangle(scaledImageData, rect, cv::Scalar(0, 255, 255, 125), 2);
    }

    cv::imshow(WINDOW_TITLE, scaledImageData);
    cv::waitKey(0);

    return 0;
}
