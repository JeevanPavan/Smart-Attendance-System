#ifndef FACE_DETECT_H
#define FACE_DETECT_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <tuple>

void loadFaceModel(const std::string& path);
bool detectAndRecognizeFace(cv::Mat& frame, std::vector<std::tuple<int, double, cv::Rect>>& results);
std::string saveUnknownFace(const cv::Mat& frame, const std::tm& t);
std::string saveCapturedFaceFrame(const cv::Mat& frame, const std::tm& t); // 👈 ADD THIS
void captureAndSaveFace(cv::Mat& frame, const std::string& personName, int imgCount);
cv::Rect getLastDetectedFaceROI();

#endif
