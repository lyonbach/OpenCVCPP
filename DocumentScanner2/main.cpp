#include <filesystem>
#include <iostream>
#include <vector>
#include <set>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define DPI 300
#define PREVIEW_HEIGHT 800
#define PREVIEW_WIDTH 640

namespace fs = std::filesystem;

struct ProcessedImageInfo
{
    std::string imageName;
    cv::Mat imageData;
};


struct ContoursInfo
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
};

std::string getTargetImageName(std::string sourceImageFullName, int imageNo=-1)
{
    if (imageNo < 0)
    {
        return std::string(fs::path(sourceImageFullName).filename());
    }
    else
    {
        std::cout << "here" << std::endl;
        std::string targetImageName;
        targetImageName = fs::path(sourceImageFullName).filename().replace_extension("");
        std::string extension = fs::path(sourceImageFullName).extension();
        targetImageName + "_" + std::to_string(imageNo);
        return targetImageName;
    }
}

void preProcessImageData(cv::Mat *imgIn, cv::Mat *imgOut)
{

    cv::Mat imgG, imgBlrG, imgBlrCn, imgBlrCnDl;

    // Convert image to grayscale
    cv::cvtColor(*imgIn, imgG, cv::COLOR_BGR2GRAY);

    // Blur image
    cv::GaussianBlur(imgG, imgBlrG, cv::Size(9, 9), 3.0);

    // Apply canny
    cv::Canny(imgBlrG, imgBlrCn, 25, 75, 3);

    // Apply dilation
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(imgBlrCn, *imgOut, kernel);
}

/** @brief Identifies the shape of the contour.
@param contour A pointer to the input vector of 2D points (contour vertices), stored in std::vector.
 */
void getContourShapes(std::vector<std::vector<cv::Point>> *contours, std::vector<std::vector<cv::Point>> *contourShapes)
{
    std::vector<cv::Point> approximation;
    float perimeter;

    for (std::vector<cv::Point> contour : *contours)
    {
        cv::arcLength(contour, true);
        perimeter = cv::arcLength(contour, true);
        approxPolyDP(contour, approximation, 0.02 * perimeter, true);
        if ((int)approximation.size() != 4)
            continue;
        contourShapes->push_back(approximation);
    }
}

void computeContoursInfo(cv::Mat *imgIn, ContoursInfo* contoursInfo, float areaThreshold=2500.0)
{

    std::vector<std::vector<cv::Point>> allContours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(*imgIn, allContours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for ( size_t i=0; i < allContours.size(); i++)
    {
        std::vector<cv::Point> contour = allContours.at(i);
        cv::Vec4i contourPoint = hierarchy.at(i);
        if (cv::contourArea(contour) < areaThreshold)
            continue;
        contoursInfo->contours.push_back(contour);
        contoursInfo->hierarchy.push_back(contourPoint);
    }

}

void reorderContourPoints(std::vector<cv::Point> *contour, std::vector<cv::Point2f> *reorderedContour)
{
    std::vector<int> summation, subtraction;
    for(cv::Point point : *contour)
    {
        summation.push_back(point.x + point.y);
        subtraction.push_back(point.x - point.y);
    }
    reorderedContour->push_back(contour->at(std::min_element(summation.begin(), summation.end()) - summation.begin()));
    reorderedContour->push_back(contour->at(std::max_element(subtraction.begin(), subtraction.end()) - subtraction.begin()));
    reorderedContour->push_back(contour->at(std::min_element(subtraction.begin(), subtraction.end()) - subtraction.begin()));
    reorderedContour->push_back(contour->at(std::max_element(summation.begin(), summation.end()) - summation.begin()));
}

std::vector<ProcessedImageInfo> processOneImage(std::string imageFullPath)
{

    // Necessary definitions
    cv::Mat imageDataOriginal, imageDataPreProcessed, imageDataFlat, imageToSave;

    // Load image data
    imageDataOriginal = cv::imread(imageFullPath, cv::IMREAD_UNCHANGED);

    // Do preprocessing.
    preProcessImageData(&imageDataOriginal, &imageDataPreProcessed);

    // Compute contours
    ContoursInfo contoursInfo;
    computeContoursInfo(&imageDataPreProcessed, &contoursInfo);

    std::vector<std::vector<cv::Point>> contourShapes;
    getContourShapes(&contoursInfo.contours, &contourShapes);

    // At this point we must have only have rectangular contours.
    std::cout << "Found " << contourShapes.size() << " document(s)." << std::endl;
    std::vector<ProcessedImageInfo> processedImages;
    int imageNo = -1;
    for(std::vector<cv::Point> contour : contourShapes)
    {
        float width = (float)imageDataPreProcessed.cols;
        float height = (float)imageDataPreProcessed.rows;

        std::vector<cv::Point2f> source_;
        reorderContourPoints(&contour, &source_);

        cv::Point2f source[4] = { source_[0], source_[1], source_[2], source_[3] };
        cv::Point2f destination[4] = { {0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}, {width, height} };
        cv::warpPerspective(imageDataOriginal, imageDataFlat, cv::getPerspectiveTransform(source, destination), cv::Point(width, height));

        // 8-1/4 x 11-3/4 # TODO IMPLEMENT OUTPUT TYPES
        int saveWidth = 11.75 * DPI;
        int saveHeight = 8.25f * DPI;


        // Scale image for saving.
        if (contourShapes.size() > 1) imageNo++;
        cv::resize(imageDataFlat, imageToSave, cv::Size(saveWidth, saveHeight));
        cv::rotate(imageToSave, imageToSave, cv::ROTATE_90_CLOCKWISE);
        processedImages.push_back({getTargetImageName(imageFullPath, imageNo), imageToSave});
    }
    return processedImages;
}

bool getUserDecision(cv::Mat* imageDataOriginal, ProcessedImageInfo *processedImageInfo)
{
    cv::Mat previewImageOriginal, previewImageProcessed;

    // First we create a window title.
    std::string windowTitle = fs::path(processedImageInfo->imageName).filename();

    // Scale image to a size to fit in the screen.
    int scaleParameter_0 = PREVIEW_WIDTH;
    float coefficient = (float)scaleParameter_0 / processedImageInfo->imageData.cols;

    int scaleParameter_1;
    scaleParameter_1 = (int)processedImageInfo->imageData.rows * coefficient;

    cv::Size sizeProcessed(scaleParameter_0, scaleParameter_1);
    cv::Size sizeOriginal(scaleParameter_1, scaleParameter_0);

    cv::resize(processedImageInfo->imageData, previewImageProcessed, sizeProcessed);
    cv::resize(*imageDataOriginal, previewImageOriginal, sizeOriginal);

    cv::rotate(previewImageOriginal, previewImageOriginal, cv::ROTATE_90_CLOCKWISE);
    cv::imshow("Original", previewImageOriginal);
    cv::imshow(windowTitle, previewImageProcessed);

    bool shouldSave = false;
    int key = cv::waitKey(0);
    switch (key)
    {
        case 'q':
            {
                cv::destroyAllWindows();
                std::exit(0);
            }

        case 's':
            shouldSave = true;
            break;

        case 'p':
            shouldSave = false;
            break;

        default:
            shouldSave = true;
            break;
    }
    cv::destroyWindow(windowTitle);
    return shouldSave;
}

std::string saveImage(ProcessedImageInfo *processedImageInfo, std::string outputFolder)
{
    if(!fs::exists(outputFolder))
    {
        fs::create_directory(outputFolder);
    }

    std::string targetImagePath = std::string(fs::path(outputFolder) / processedImageInfo->imageName);
    std::vector<int> imageWriteParameters;
    imageWriteParameters.push_back(cv::IMWRITE_JPEG_QUALITY);
    imageWriteParameters.push_back(100);
    cv::imwrite(targetImagePath, processedImageInfo->imageData, imageWriteParameters);
    return targetImagePath;
}

int main(int argc, char *argv[])
{

    std::string inputFolder, outputFolder;
    bool interactiveMode = false;

    if (argc < 3)
    {
        std::cout << "At least two arguments are needed." << std::endl;
        return 1;
    }
    if (argc == 4)
    {
        if ((std::string)argv[3] == "true")
        {
            interactiveMode = true;
        }
        else if ((std::string)argv[3] == "false")
        {
            interactiveMode = false;
        }
        else
        {
            std::cout << "Last argument must be \"true\" or \"false\"!\n";
            return 1;
        }
    }

    // Get arguments
    inputFolder = argv[1];
    outputFolder = argv[2];

    // Find all pictures in the folder, sorted
    std::set<fs::path> sortedImages;
    for (auto &item : fs::directory_iterator(inputFolder))
    {
        if(item.is_directory()) continue;
        sortedImages.insert(item.path());
    }

    // Process all items.
    for (auto &imageFullPath : sortedImages)
    {
        std::cout << "Finding documents in:\n\t" << imageFullPath << std::endl;
        // One photo could contain more than one processed image (document).
        std::vector<ProcessedImageInfo> processedImages = processOneImage(imageFullPath);
        for (ProcessedImageInfo processedImageInfo : processedImages)
        {
            if (processedImageInfo.imageData.empty())
            {
                std::cout << "Could not process:\n" << processedImageInfo.imageName << "\nskipping..." << std::endl;
                continue;
            }

            bool shouldSaveImage = true;
            // If interactive mode was selected, shouldSaveImage parameter will be changed with the 
            // following function call.
            if (interactiveMode)
            {
                cv::Mat imageDataOriginal = cv::imread(imageFullPath, cv::IMREAD_UNCHANGED);
                shouldSaveImage = getUserDecision(&imageDataOriginal, &processedImageInfo);
            }

            // If should not save iamge skip this one.
            if(!shouldSaveImage)
            {
                std::cout << "Skipping:\n\t" << processedImageInfo.imageName << std::endl;
                continue;
            }
    
            // If everything is alright, save image.
            std::cout << "Saving image..." << std::endl;
            std::string savedImage = saveImage(&processedImageInfo, outputFolder);
            std::cout << "Saved image as:\n\t" << savedImage << std::endl;
        }

    }




    return 0;
}
