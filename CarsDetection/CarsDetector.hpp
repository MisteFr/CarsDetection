//
//  CarsDetector.hpp
//  CarsDetection
//
//  Created by Anurag Ajwani on 28/04/2019.
//  Copyright © 2019 Anurag Ajwani. All rights reserved.
//  Modified by Arthur Bigot


/**
 The bridging header file is important as it will allow us to consume our cars detector algorithm by allowing different languages to talk to each other. 
 */

#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>

using namespace cv;
using namespace std;

struct DetectionResult {
    std::vector<cv::Point> yellowDP;
    std::vector<cv::Point> greenDP;
    std::vector<cv::Point> redDP;
    Mat maskedImage;
    int imageWidth;
    int imageHeight;
};

struct dataPoint {
    cv::Point point;
    int width;
    int height;
};

class CarsDetector {
    
    public:
    
    /*
     Returns image with cars overlay
     */
    DetectionResult detect_cars(Mat image);
    
    /*
     Returns image with corners overlay
     */
    DetectionResult detect_corners(Mat image);

    
    private:
    
    /*
     Filters yellow and white colors on image
     */
    DetectionResult detect_cars_f(Mat image);
    
    /*
     Filters yellow and white colors on image
     */
    DetectionResult detect_corners_f(Mat image);
    
    std::vector<dataPoint> detectObjects(cv::Mat Mask);
    
    Mat drawBoxes(cv::Mat Mask, std::vector<dataPoint> p, Scalar color);
    
    Mat drawRectangles(cv::Mat Mask, std::vector<dataPoint> p, Scalar color);
    
};
