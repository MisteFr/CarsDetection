//
//  CarsDetectorBridge.h
//  CarsDetection
//
//  Created by Anurag Ajwani on 28/04/2019.
//  Copyright © 2019 Anurag Ajwani. All rights reserved.
//  Modified by Arthur Bigot

/**
 Our Swift code is not able to consume C++ code (at least not at the time of writing). However Objective-C is. Furthermore we can consume Objective-C code through Swift. So let’s create Objective-C code to bridge between Swift and C++.
 
 Here basically we are declaring a single method in our CarsDetectorBridge class which will take an UIImage instance and return a UIImage instance with the cars overlayed.
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface CarsDetectorBridge : NSObject
    
- (NSDictionary *) detectCarsIn: (UIImage *) image;

- (NSDictionary *) detectCornersIn: (UIImage *) image;
    
@end
