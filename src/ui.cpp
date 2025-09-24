#include "ui.h"
#include <opencv2/opencv.hpp>

void displayLiveCount(cv::Mat& frame, int count) {
    cv::putText(frame, "Today's Count: " + std::to_string(count),
        cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8,
        cv::Scalar(0, 255, 255), 2);
}

void drawLabel(cv::Mat& frame, const std::string& text, const cv::Scalar& color) {
    cv::putText(frame, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.9, color, 2);
}
