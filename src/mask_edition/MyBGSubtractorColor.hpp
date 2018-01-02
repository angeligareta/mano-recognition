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
#include <algorithm>

#define MAX_HORIZ_SAMPLES 3
#define MAX_VERT_SAMPLES 6
#define SAMPLE_SIZE 20
#define DISTANCE_BETWEEN_SAMPLES 30

class MyBGSubtractorColor {
public:

    MyBGSubtractorColor(cv::VideoCapture vc);
    void LearnModel();
    void ObtainBGMask(cv::Mat frame, cv::Mat &bgmask);

private:
    int h_low, h_up, l_low, l_up, s_low, s_up;
    int max_samples;

    cv::VideoCapture cap;
    std::vector<cv::Scalar> means;
    std::vector<cv::Scalar> lower_bounds;
    std::vector<cv::Scalar> upper_bounds;
    static void Trackbar_func(int, void*);
};
