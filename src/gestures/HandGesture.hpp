#pragma once

#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

class HandGesture {
public:
    HandGesture();
    std::vector<cv::Point> FeaturesDetection(cv::Mat mask, cv::Mat output_img, double &countour_area);

private:
    double getAngle(cv::Point s, cv::Point e, cv::Point f);

};
