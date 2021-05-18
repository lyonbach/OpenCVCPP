#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

int main(int argc, char const *argv[])
{

    float width=240, height=580;
    cv::Mat imageDataFlat, transformationMatrix;

    std::string imagePath = "../../SampleImages/ingenuitiy_closup.jpg";
    cv::Mat imageData = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    

    // Source points define the corners of the distorted image.
    cv::Point2f source[4] = { {698, 205}, {872, 201}, {727, 375}, {918, 370} };


    // Destination points define the corners of the flattened image.
    cv::Point2f destination[4] = { {0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}, {width, height} };

    transformationMatrix = cv::getPerspectiveTransform(source, destination);
    cv::warpPerspective(imageData, imageDataFlat, transformationMatrix, cv::Point(width, height));

    // Draw a rectangle to show if we select the correct points.
    cv::line(imageData, source[0], source[2], cv::Vec4i(0, 255, 0, 255), 3);
    cv::line(imageData, source[1], source[3], cv::Vec4i(255, 0, 0, 255), 3);

    // Show images.
    cv::imshow("Ingenuity", imageData);
    cv::imshow("SolarPanel", imageDataFlat);
    cv::waitKey(0);
    
    return 0;
}

