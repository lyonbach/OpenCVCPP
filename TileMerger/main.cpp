#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, char *argv[])
{

    std::vector<std::tuple<cv::String, int, int>> listOfImages;

    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_0.jpg", 0, 0);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_1.jpg", 1, 0);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_2.jpg", 2, 0);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_3.jpg", 0, 1);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_4.jpg", 1, 1);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_5.jpg", 2, 1);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_6.jpg", 0, 2);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_7.jpg", 1, 2);
    listOfImages.emplace_back("/media/lyonbach/depot/Work/Projects/GeneralLearningPractice/image_merge/resources/test_pattern_8.jpg", 2, 2);

    int totalWidth = 0;
    int totalHeight = 0;
    int maxCol = -1;
    int maxRow = -1;
    int imageDataType = NULL;
    int margin = 50;
    for (auto item : listOfImages)  // First loop is to identify the total output matrix size.
    {
        cv::Mat imageData = cv::imread(std::get<0>(item), cv::IMREAD_UNCHANGED);
        int col = std::get<1>(item);
        int row = std::get<2>(item);

        std::cout << "Reading image: " << std::get<0>(item) << std::endl;

        if (imageDataType == NULL)
        {
            imageDataType = imageData.type();
        }

        if (row > maxRow)
        {
            maxRow = row;
            totalHeight += imageData.rows - 2*margin;
        }

        if (col > maxCol)
        {
            maxCol = col;
            totalWidth += imageData.cols - 2*margin;
        }

    }

    std::cout << "Total Width: " << totalWidth << std::endl;
    std::cout << "Total Height: " << totalHeight << std::endl;

    // Second loop is to really merge the image parts
    cv::Mat outputMatrix = cv::Mat::zeros(totalHeight, totalWidth, imageDataType);
    cv::Mat additionMatrix = cv::Mat::zeros(totalHeight, totalWidth, imageDataType);
    int startX = 0; int startY = 0; int endX = 0; int endY = 0; int currentCol = 0; int currentRow = 0;
    for (auto item : listOfImages)  // First loop is to identify the total output matrix size.
    {
        cv::Mat imageData = cv::imread(std::get<0>(item), cv::IMREAD_UNCHANGED);
        int col = std::get<1>(item);
        int row = std::get<2>(item);

        if (col == 0)
        {
            startX = 0;
            cv::Rect roi(margin, 0, imageData.cols - margin, imageData.rows);
            imageData = imageData(roi);
            endX = startX + imageData.cols;
        }
        else if (col == maxCol)
        {
            cv::Rect roi(0, 0, imageData.cols - margin, imageData.rows);
            imageData = imageData(roi);
            startX = additionMatrix.cols - imageData.cols;
            endX = additionMatrix.cols;
        }
        else if (col > currentCol)
        {
            startX = endX - 2 * margin;
            endX = startX + imageData.cols;
        }
        currentCol = col;
        if (row == 0)
        {
            startY = 0;
            cv::Rect roi(0, margin, imageData.cols, imageData.rows - margin);
            imageData = imageData(roi);
            endY = startY + imageData.rows;
        }
        else if (row == maxRow)
        {
            cv::Rect roi(0, 0, imageData.cols, imageData.rows - margin);
            imageData = imageData(roi);
            startY = additionMatrix.rows - imageData.rows;
            endY = additionMatrix.rows;
        }
        else if (row > currentRow)
        {
            startY = endY - 2 * margin;
            endY = startY + imageData.rows;
        }
        currentRow = row;

        std::cout << "----------------------------------" << std::endl;
        std::cout << startX << std::endl;
        std::cout << startY << std::endl;
        std::cout << endX << std::endl;
        std::cout << endY << std::endl;
        std::cout << "----------------------------------" << std::endl;

        cv::Rect roi(startX, startY, endX - startX, endY - startY);
        additionMatrix = additionMatrix(roi);
    }

    //     outputMatrix += additionMatrix;

    //     cv::imshow("test", outputMatrix);
    //     cv::waitKey(0);
    // }

    return 0;
}
