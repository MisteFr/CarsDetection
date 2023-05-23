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

           if (boundingBox.area() > 1000 && approx.size() == 3) {
               // get center point of bounding box
               Point centerPoint = Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
               
               double dist1 = norm(approx[0] - approx[1]);
               double dist2 = norm(approx[1] - approx[2]);
               double dist3 = norm(approx[2] - approx[0]);

               // Determine the smallest side of the triangle
               Point pt1, pt2;
               if (dist1 <= dist2 && dist1 <= dist3) {
                   pt1 = approx[0];
                   pt2 = approx[1];
               } else if (dist2 <= dist1 && dist2 <= dist3) {
                   pt1 = approx[1];
                   pt2 = approx[2];
               } else {
                   pt1 = approx[2];
                   pt2 = approx[0];
               }
               

               // Calculate the orientation of the line in degrees
               double dx = pt2.x - pt1.x;
               double dy = pt2.y - pt1.y;
               double angle = atan2(dy, dx) * 180.0 / CV_PI;
               angle = -angle;
               if (angle < 0) {
                   angle += 360.0;
               }
               

               // add center point to detectedPoints vector along with shape information
               dataPoint p;
               p.width = boundingBox.width;
               p.height = boundingBox.height;
               p.point = centerPoint;
               p.angle = angle;

               // detect shape based on the number of vertices in the polygon approximation
               // triangle shape
               detectedData.push_back(p);
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
    Scalar yellowLower = Scalar(20, 70, 70);
    Scalar yellowUpper = Scalar(30, 255, 255);
    inRange(hlsColorspacedImage, yellowLower, yellowUpper, yellowMask);
     
    std::vector<dataPoint> yellowPoints = detectObjects(yellowMask);
    // combine masks into a single image
    Mat colorMask = yellowMask;
     
    
    /*
     when we tested for red corners intead of yellow
    Mat redMask1;
    Scalar redLower1 = Scalar(165, 50, 50); // lower boundary for first hue range (330-360)
    Scalar redUpper1 = Scalar(180, 255, 255);
    inRange(hlsColorspacedImage, redLower1, redUpper1, redMask1);
    
    Mat redMask2;
    Scalar redLower2 = Scalar(0, 50, 50); // lower boundary for second hue range (0-30)
    Scalar redUpper2 = Scalar(15, 255, 255); // upper boundary for second hue range (150-180)
    inRange(hlsColorspacedImage, redLower2, redUpper2, redMask2);
    
    Mat redMask = redMask1 + redMask2;
    
    std::vector<dataPoint> yellowPoints = detectObjects(redMask);
     
     // combine masks into a single image
     Mat colorMask = redMask;
     */
    
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, colorMask);
    
    if (yellowPoints.size() == 4) {
        drawRectangles(maskedImage, yellowPoints, Scalar(255, 255, 0));
    }
    
    drawBoxes(maskedImage, yellowPoints, Scalar(255, 255, 0));
    
    // create a new vector of Point
    /*
    std::vector<cv::Point> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
     */
    std::vector<std::tuple<cv::Point, double>> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return std::make_tuple(dp.point, dp.angle);
    });
    
    //std::vector<cv::Point> gP;
    //std::vector<cv::Point> bP;
    
    std::vector<std::tuple<cv::Point, double>> gP;
    std::vector<std::tuple<cv::Point, double>> bP;
    
    result.yellowDP = yP;
    result.greenDP = gP;
    result.blueDP = bP;
    
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
    Scalar yellowLower = Scalar(20, 70, 70);
    Scalar yellowUpper = Scalar(30, 255, 255);
    inRange(hlsColorspacedImage, yellowLower, yellowUpper, yellowMask);
    
    std::vector<dataPoint> yellowPoints = detectObjects(yellowMask);
    
    //idée c'est itérer sur chaque mask de couleur et save le centre Yellow, Green et Red
    // find contours of shapes in the image
    
    /*
    Mat greenMask;
    
    Scalar greenLower = Scalar(29, 86, 6); // lower boundary for green hue range
    Scalar greenUpper = Scalar(64, 255, 255);
    
    inRange(hlsColorspacedImage, greenLower, greenUpper, greenMask);
    
    std::vector<dataPoint> greenPoints = detectObjects(greenMask);
     */
    
    Mat blueMask;
    
    Scalar blueLower = Scalar(90, 30, 100);
    Scalar blueUpper = Scalar(150, 255, 255);
    
    inRange(hlsColorspacedImage, blueLower, blueUpper, blueMask);
    
    std::vector<dataPoint> bluePoints = detectObjects(blueMask);
    
    Mat greenMask;
    
    Scalar greenLower = Scalar(30, 40, 50);
    Scalar greenUpper = Scalar(90, 255, 255);
    inRange(hlsColorspacedImage, greenLower, greenUpper, greenMask);
    
     
    
    std::vector<dataPoint> greenPoints = detectObjects(greenMask);
    
    // combine masks into a single image
    Mat colorMask = yellowMask + blueMask + greenMask;
    
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, colorMask);
    
    drawBoxes(maskedImage, yellowPoints, Scalar(255, 255, 0));
    drawBoxes(maskedImage, bluePoints, Scalar(0, 0, 255));
    drawBoxes(maskedImage, greenPoints, Scalar(0, 255, 0));
    
    // create a new vector of Point
    /*
    std::vector<cv::Point> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    std::vector<cv::Point> gP(greenPoints.size());
    std::transform(greenPoints.begin(), greenPoints.end(), gP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
    
    std::vector<cv::Point> bP(bluePoints.size());
    std::transform(bluePoints.begin(), bluePoints.end(), bP.begin(), [](const dataPoint& dp) {
        return dp.point;
    });
     */
    
    std::vector<std::tuple<cv::Point, double>> yP(yellowPoints.size());
    std::transform(yellowPoints.begin(), yellowPoints.end(), yP.begin(), [](const dataPoint& dp) {
        return std::make_tuple(dp.point, dp.angle);
    });
    
    std::vector<std::tuple<cv::Point, double>> gP(greenPoints.size());
    std::transform(greenPoints.begin(), greenPoints.end(), gP.begin(), [](const dataPoint& dp) {
        return std::make_tuple(dp.point, dp.angle);
    });
    
    std::vector<std::tuple<cv::Point, double>> bP(bluePoints.size());
    std::transform(bluePoints.begin(), bluePoints.end(), bP.begin(), [](const dataPoint& dp) {
        return std::make_tuple(dp.point, dp.angle);
    });
    
    result.yellowDP = yP;
    result.greenDP = gP;
    result.blueDP = bP;
    
    result.maskedImage = maskedImage;
    result.imageWidth = maskedImage.rows;
    result.imageHeight = maskedImage.cols;
        
    return result;
}
