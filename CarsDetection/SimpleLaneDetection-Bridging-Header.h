//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

/**
 One more step before we can consume CarsDetectorBridge from our Swift code is to tell Xcode to make that class accessible to Swift. We do so by declaring the header files to be accessible in our bridging file. Open SimpleLaneDetection-Bridging-Header.h and add the following line:
 */

#import "CarsDetectorBridge.h"
