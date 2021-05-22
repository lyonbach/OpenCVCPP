#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <iostream>
#include <filesystem>

#define CLASSIFIER_PATH "/home/lyonbach/Projects/OpenCV/OpenCVCPP/LicensePlateDetector/russian_plate_number.xml"
#define VIZ_HEIGHT 800
namespace fs = std::filesystem;

std::string getParameters(int argc, const char *argv[])
{
    const std::string keys =
        "{help h usage ? |      | Usage.}"
        "{@image         |      | Image full path to search license plates in.}"
        ;

    // Get commandline parameters and check them.
    cv::CommandLineParser parser(argc, argv, keys);

    if (!parser.check())
    {
        parser.printErrors();
        return "";
    }

    std::string imagePath = parser.get<cv::String>("@image");
    if(imagePath.empty())
    {
        parser.printMessage();
        return "";
    }

    if (parser.has("help"))
    {
        parser.printMessage();
        return "";
    }

    if (!fs::exists(imagePath))
    {
        std::cout << "Given path does not exists!\nPlease make sure that you give a valid image path." << std::endl;
        return "";
    }

    return imagePath;
}

int main(int argc, char const *argv[])
{
    // Variable declarations.
    std::vector<cv::Rect> plates;
    cv::CascadeClassifier classifier;

    classifier.load(CLASSIFIER_PATH);
    if (classifier.empty())
    {
        std::cout << "Unable to load the classifier!" << std::endl;
        return 1;
    }

    std::string imagePath = getParameters(argc, argv);
    if (imagePath.empty())
    {
        return 1;
    }

    // Read image.
    cv::Mat imageData = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    // cv::Mat imageData = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

    // Detect plates.
    classifier.detectMultiScale(imageData, plates, 1.2, 3, 0, cv::Size(), cv::Size());

    // Draw detected plates as rectangles.
    for (cv::Rect boundingRect : plates)
    {
        cv::rectangle(imageData, boundingRect, cv::Scalar(0, 255, 255), 3);
    }
    
    // Scale image for visualising purposes.
    float coef = (float)VIZ_HEIGHT / imageData.size().height;
    cv::resize(imageData, imageData, cv::Size(imageData.size().width * coef, VIZ_HEIGHT));

    cv::imshow("Detection", imageData);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
