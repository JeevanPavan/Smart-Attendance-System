#ifndef UI_H
#define UI_H

#include <opencv2/core.hpp>
void displayLiveCount(cv::Mat& frame, int count);
void drawLabel(cv::Mat& frame, const std::string& text, const cv::Scalar& color);

#endif
