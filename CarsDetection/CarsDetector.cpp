//
//  CarsDetector.cpp
//  CarsDetection
//
//  Created by Anurag Ajwani on 28/04/2019.
//  Copyright © 2019 Anurag Ajwani. All rights reserved.
//  Modified by Arthur Bigot

#include "CarsDetector.hpp"

using namespace cv;
using namespace std;


DetectionResult CarsDetector::detect_cars(Mat image) {
    DetectionResult detectionResult = detect_cars_f(image);
    return detectionResult;
}

DetectionResult CarsDetector::detect_corners(Mat image) {
    DetectionResult detectionResult = detect_corners_f(image);
    return detectionResult;
}

std::vector<dataPoint> CarsDetector::detectObjects(cv::Mat Mask){
    // find contours of shapes in the image
    vector<vector<Point>> contours;
    findContours(Mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // vector to hold the detected points
    std::vector<dataPoint> detectedData;

    // loop through each contour and draw bounding boxes around the shapes
    for (size_t i = 0; i < contours.size(); i++) {
        
        // get the contour's perimeter
       double perimeter = arcLength(contours[i], true);

       // approximate the contour as a polygon
       vector<Point> approx;
       approxPolyDP(contours[i], approx, 0.04 * perimeter, true);

       // get bounding box coordinates for contour
       Rect boundingBox = boundingRect(contours[i]);

       if (boundingBox.area() > 200) {
           // get center point of bounding box
           Point centerPoint = Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);

           // add center point to detectedPoints vector along with shape information
           dataPoint p;
           p.width = boundingBox.width;
           p.height = boundingBox.height;
           p.point = centerPoint;

           // detect shape based on the number of vertices in the polygon approximation
           // triangle shape
           if (approx.size() == 3) {
               detectedData.push_back(p);
           }
       }
    }
    return detectedData;
}

Mat CarsDetector::drawBoxes(cv::Mat Mask, std::vector<dataPoint> p, Scalar color){
    
    for (dataPoint center : p) {
        Point topLeft = center.point - Point(center.width / 2, center.height / 2);
        Rect boundingBox(topLeft, Size(center.width, center.height));
        rectangle(Mask, boundingBox, color, 2);
    }
    
    return Mask;
}

Mat CarsDetector::drawRectangles(cv::Mat Mask, std::vector<dataPoint> p, Scalar color){
    Point pt1(p[0].point.x, p[0].point.y);
    Point pt2(p[1].point.x, p[1].point.y);
    Point pt3(p[2].point.x, p[2].point.y);
    Point pt4(p[3].point.x, p[3].point.y);
    
    cv::rectangle(Mask, pt1, pt3, color, 2);
    
    return Mask;
}

//filter colors of our images - should work
DetectionResult CarsDetector::detect_corners_f(Mat image) {
    DetectionResult result;
    
    Mat hlsColorspacedImage;
    cvtColor(image, hlsColorspacedImage, CV_RGB2HLS);
    
    Mat yellowMask;
    //Scalar yellowLower = Scalar(10, 0, 90);
    //Scalar yellowUpper = Scalar(50, 255, 255);
    Scalar yellowLower = Scalar(20, 70, 70);
    Scalar yellowUpper = Scalar(30, 255, 255);
    inRange(hlsColorspacedImage, yellowLower, yellowUpper, yellowMask);
    
    std::vector<dataPoint> yellowPoints = detectObjects(yellowMask);
    
    
    
    // combine masks into a single image
    Mat colorMask = yellowMask;
    
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, colorMask);
    
    if (yellowPoints.size() == 4) {
        drawRectangles(maskedImage, yellowPoints, Scalar(255, 255, 0));
    }
    
    drawBoxes(maskedImage, yellowPoints, Scalar(255, 255, 0));
    
    // create a new vector of Point
    std::vector<cv::Point> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    std::vector<cv::Point> gP;
    std::vector<cv::Point> rP;
    
    result.yellowDP = yP;
    result.greenDP = gP;
    result.redDP = rP;
    
    result.maskedImage = maskedImage;
    result.imageWidth = maskedImage.rows;
    result.imageHeight = maskedImage.cols;
        
    return result;
}

//filter colors of our images - should work
DetectionResult CarsDetector::detect_cars_f(Mat image) {
    DetectionResult result;
    
    Mat hlsColorspacedImage;
    cvtColor(image, hlsColorspacedImage, CV_RGB2HLS);
    
    Mat yellowMask;
    //Scalar yellowLower = Scalar(10, 0, 90);
    //Scalar yellowUpper = Scalar(50, 255, 255);
    Scalar yellowLower = Scalar(20, 70, 70);
    Scalar yellowUpper = Scalar(30, 255, 255);
    inRange(hlsColorspacedImage, yellowLower, yellowUpper, yellowMask);
    
    std::vector<dataPoint> yellowPoints = detectObjects(yellowMask);
    
    //idée c'est itérer sur chaque mask de couleur et save le centre Yellow, Green et Red
    // find contours of shapes in the image
    
    Mat greenMask;
    Scalar greenLower = Scalar(29, 86, 6); // lower boundary for green hue range
    Scalar greenUpper = Scalar(64, 255, 255);
    inRange(hlsColorspacedImage, greenLower, greenUpper, greenMask);
    
    std::vector<dataPoint> greenPoints = detectObjects(greenMask);
    
    Mat redMask1;
    Scalar redLower1 = Scalar(165, 50, 50); // lower boundary for first hue range (330-360)
    Scalar redUpper1 = Scalar(180, 255, 255);
    inRange(hlsColorspacedImage, redLower1, redUpper1, redMask1);
    
    Mat redMask2;
    Scalar redLower2 = Scalar(0, 50, 50); // lower boundary for second hue range (0-30)
    Scalar redUpper2 = Scalar(15, 255, 255); // upper boundary for second hue range (150-180)
    inRange(hlsColorspacedImage, redLower2, redUpper2, redMask2);
    
    Mat redMask = redMask1 + redMask2;
    
    std::vector<dataPoint> redPoints = detectObjects(redMask);
    
    // combine masks into a single image
    Mat colorMask = yellowMask + greenMask + redMask;
    
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, colorMask);
    
    drawBoxes(maskedImage, yellowPoints, Scalar(255, 255, 0));
    drawBoxes(maskedImage, greenPoints, Scalar(0, 255, 0));
    drawBoxes(maskedImage, redPoints, Scalar(255, 0, 0));
    
    // create a new vector of Point
    std::vector<cv::Point> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    std::vector<cv::Point> rP(redPoints.size());
    std::transform(redPoints.begin(), redPoints.end(), rP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    std::vector<cv::Point> gP(greenPoints.size());
    std::transform(greenPoints.begin(), greenPoints.end(), gP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    result.yellowDP = yP;
    result.greenDP = gP;
    result.redDP = rP;
    
    result.maskedImage = maskedImage;
    result.imageWidth = maskedImage.rows;
    result.imageHeight = maskedImage.cols;
        
    return result;
}
