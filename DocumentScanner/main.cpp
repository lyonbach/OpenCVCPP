#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>

const char* imagePath;

void _imShow(cv::Mat &image, cv::String windowName="Image")
{
    cv::imshow(windowName, image);
    while(cv::waitKey(0) != 113){};
    cv::destroyWindow(windowName);
}

std::vector<cv::Point> getSortedPoints(std::vector<cv::Point> contour)
{
    cv::Point topLeft;
    cv::Point topRight;
    cv::Point botLeft;
    cv::Point botRight;

    int pointCount = contour.size();
    int xs[pointCount];
    int ys[pointCount];

    std::vector<cv::Point> corners;

    for (int i = 0; i < pointCount; i++)
    {
        xs[i] = contour.at(i).x;
        ys[i] = contour.at(i).y;
    }

    topLeft.x  = *std::min_element(xs, xs + pointCount);
    topLeft.y  = *std::min_element(ys, ys + pointCount);
    corners.push_back(topLeft);

    topRight.x = *std::max_element(xs, xs + pointCount);
    topRight.y = *std::min_element(ys, ys + pointCount);
    corners.push_back(topRight);

    botRight.x = *std::max_element(xs, xs + pointCount);
    botRight.y = *std::max_element(ys, ys + pointCount);
    corners.push_back(botRight);

    botLeft.x  = *std::min_element(xs, xs + pointCount);
    botLeft.y  = *std::max_element(ys, ys + pointCount);
    corners.push_back(botLeft);

    return corners;
}

cv::Mat warp(std::vector<cv::Point> corners, cv::Mat source, _Float32 width=-1.0f, _Float32 height=-1.0f)
{
    cv::Mat warped;
    cv::Mat mapMatrix;
    
    if (width == -1.0f)
        width = (_Float32) source.size[0];
    if (height == -1.0f)
        height = (_Float32) source.size[1];
    

    std::vector<cv::Point2f> sourceCorners;
    for (int i = 0; i < corners.size(); i++)
    {
        cv::Point p = corners[i];
        sourceCorners.push_back(cv::Point2f(p.x, p.y));
    }

    // Construct a vector of points that vector of original corners points corresponds.
    std::vector<cv::Point2f> targetCorners {
        cv::Point(0.0f, 0.0f),     // Top Left
        cv::Point(width, 0.0f),    // Top Right
        cv::Point(width, height),  // Bot Right
        cv::Point(0.0f, height)    // Bot Left
    };

    mapMatrix = cv::getPerspectiveTransform(sourceCorners, targetCorners);
    std::cout << mapMatrix << std::endl;

    // Warp the original image.
    cv::warpPerspective(source, warped, mapMatrix, cv::Size(width, height));

    return warped;
}

int main(int argc, char const *argv[])
{
    cv::Mat imageOriginal;
    cv::Mat imageGrayScale;
    cv::Mat imageBlurred;
    cv::Mat imageEdged;
    cv::Mat imageContours;
    cv::Mat imageWarped;

    if (argc <= 1)
    {
        std::cout << "Usage: DocumentScanner <Image full path>" << std::endl;
        return 1;
    }

    imagePath = argv[1];

    std::cout << "Reading document..." << std::endl;
    imageOriginal = cv::imread(argv[1], cv::IMREAD_UNCHANGED);  // Read the file.
    if (imageOriginal.empty())
    {
        std::cout << "Could not read image content." << std::endl;
    }

    _imShow(imageOriginal);  // TEMP!

    std::cout << "Step 1: Edge detection." << std::endl;
    cv::cvtColor(imageOriginal, imageGrayScale, cv::COLOR_BGR2GRAY);  // Convert to grayscale.

    cv::GaussianBlur(imageGrayScale,  // Input image.
                     imageBlurred,    // Output image.
                     cv::Size(5, 5),  // Smoothing window width and height in pixels.
                     1.5);            // Sigma value, determines how much the image will be blurred.

    cv::Canny(imageBlurred,            // Input image.
              imageEdged,              // Output image.
              100,                     // Low threshold.
              200);                    // High threshold.

    _imShow(imageEdged, "Edges");
    std::cout << "Step 2: Find Contours." << std::endl;
    std::vector<std::vector<cv::Point>> contours_;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(imageEdged, contours_, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // cv::findContours(imageEdged, contours_, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "Step 3: Detect Contours." << std::endl;
    contours.resize(contours_.size());
    for( size_t k = 0; k < contours_.size(); k++ )
    {
        double epsilon = cv::arcLength(contours_[k], true) * 0.02f;
        cv::approxPolyDP(cv::Mat(contours_[k]), contours[k], epsilon, true);
    }

    imageContours = cv::Mat::zeros(imageGrayScale.size(), CV_8UC1);
    cv::drawContours(imageContours, contours, -1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);


    _imShow(imageContours, "Contours");

    std::cout << "Step 4: Create Birds-Eye-Image." << std::endl;

    std::vector<cv::Point> corners = getSortedPoints(contours[0]);
    imageWarped = warp(corners, imageContours);

    _imShow(imageWarped, "Warped");

    // Contours must be sorted to determine the corners.

    return 0;
}
