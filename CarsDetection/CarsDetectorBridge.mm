//
//  CarsDetectorBridge.m
//  CarsDetection
//
//  Created by Anurag Ajwani on 28/04/2019.
//  Copyright © 2019 Anurag Ajwani. All rights reserved.
//  Modified by Arthur Bigot

#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <Foundation/Foundation.h>
#import "CarsDetectorBridge.h"
#include "CarsDetector.hpp"

@implementation CarsDetectorBridge

/**
 CarsDetectorBridge converts UIImages into OpenCV image representation. Then it runs lane detection which returns an image with lane overlayed on top of it. And finally converts the OpenCV image representation back to UIImage.
 
 */
    
- (NSDictionary *) detectCarsIn: (UIImage *) image {
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    
    // convert colorspace to the one expected by the lane detector algorithm (RGB)
    cv::Mat convertedColorSpaceImage;
    cv::cvtColor(opencvImage, convertedColorSpaceImage, CV_RGBA2RGB);
    
    // Run lane detection
    CarsDetector laneDetector;
    DetectionResult imageWithLaneDetected = laneDetector.detect_cars(convertedColorSpaceImage);
    
    // convert mat to uiimage
    UIImage *maskedImage = MatToUIImage(imageWithLaneDetected.maskedImage);
    
    NSMutableArray *yellowDP = [[NSMutableArray alloc] init];
    for (cv::Point2f point : imageWithLaneDetected.yellowDP) {
        NSValue *value = [NSValue valueWithCGPoint:CGPointMake(point.x, point.y)];
        [yellowDP addObject:value];
    }
    
    NSMutableArray *greenDP = [[NSMutableArray alloc] init];
    for (cv::Point2f point : imageWithLaneDetected.greenDP) {
        NSValue *value = [NSValue valueWithCGPoint:CGPointMake(point.x, point.y)];
        [greenDP addObject:value];
    }
    
    NSMutableArray *redDP = [[NSMutableArray alloc] init];
    for (cv::Point2f point : imageWithLaneDetected.redDP) {
        NSValue *value = [NSValue valueWithCGPoint:CGPointMake(point.x, point.y)];
        [redDP addObject:value];
    }
    
    // return maskedImage and detectedPoints
    return @{@"maskedImage": maskedImage,
             @"yellowDP": yellowDP,
             @"greenDP": greenDP,
             @"redDP": redDP,
             @"imageWidth": @(imageWithLaneDetected.imageWidth),
             @"imageHeight": @(imageWithLaneDetected.imageHeight)};
}
    

- (NSDictionary *) detectCornersIn: (UIImage *) image {
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    
    // convert colorspace to the one expected by the lane detector algorithm (RGB)
    cv::Mat convertedColorSpaceImage;
    cv::cvtColor(opencvImage, convertedColorSpaceImage, CV_RGBA2RGB);
    
    // Run lane detection
    CarsDetector laneDetector;
    DetectionResult imageWithLaneDetected = laneDetector.detect_corners(convertedColorSpaceImage);
    
    // convert mat to uiimage
    UIImage *maskedImage = MatToUIImage(imageWithLaneDetected.maskedImage);
    
    NSMutableArray *yellowDP = [[NSMutableArray alloc] init];
    for (cv::Point2f point : imageWithLaneDetected.yellowDP) {
        NSValue *value = [NSValue valueWithCGPoint:CGPointMake(point.x, point.y)];
        [yellowDP addObject:value];
    }
    
    NSMutableArray *greenDP = [[NSMutableArray alloc] init];
    NSMutableArray *redDP = [[NSMutableArray alloc] init];
    
    // return maskedImage and detectedPoints
    return @{@"maskedImage": maskedImage,
             @"yellowDP": yellowDP,
             @"greenDP": greenDP,
             @"redDP": redDP,
             @"imageWidth": @(imageWithLaneDetected.imageWidth),
             @"imageHeight": @(imageWithLaneDetected.imageHeight)};
}
    
    @end
