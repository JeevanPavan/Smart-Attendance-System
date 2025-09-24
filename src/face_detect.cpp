// ===== Modified face_detect.cpp =====
#include "face_detect.h"
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <ctime>
#include <filesystem>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::face;

static Ptr<LBPHFaceRecognizer> model;
static CascadeClassifier face_cascade;
static Rect lastDetectedFace;

void loadFaceModel(const std::string& path) {
    try {
        model = LBPHFaceRecognizer::create();
        model->read(path);
        cout << "Model loaded successfully from " << path << endl;
    }
    catch (const cv::Exception& e) {
        cerr << "Error loading model: " << e.what() << endl;
    }

    if (!face_cascade.load("E:/SmartAttendance/SmartAttendance/models/haarcascade_frontalface_default.xml")) {
        cerr << "Error: Failed to load Haar cascade. Check path!" << endl;
        exit(EXIT_FAILURE);
    }
}

bool detectAndRecognizeFace(cv::Mat& frame, std::vector<std::tuple<int, double, cv::Rect>>& results) {
    results.clear();
    vector<Rect> faces;
    Mat gray;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, Size(30, 30));

    if (faces.empty() || model.empty())
        return false;

    const auto& face = faces[0];
    Mat faceROI = gray(face);
    resize(faceROI, faceROI, Size(200, 200));
    int label;
    double confidence;
    model->predict(faceROI, label, confidence);
    results.emplace_back(label, confidence, face);

    rectangle(frame, face, Scalar(255, 0, 0), 2);
    putText(frame, "ID: " + to_string(label), Point(face.x, face.y - 10),
        FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);

    // Save known face full-frame as "capture_TIMESTAMP.jpg"
    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    if (label > 0) {
        saveCapturedFaceFrame(frame, localTime);
    }

    return true;
}

std::string saveUnknownFace(const Mat& frame, const std::tm& t) {
    char filename[100];
    strftime(filename, sizeof(filename), "unknown_%Y%m%d_%H%M%S.jpg", &t);
    std::string filepath = "E:/SmartAttendance/SmartAttendance/unknown/" + std::string(filename);
    imwrite(filepath, frame);
    std::cout << "✅ Saved unknown face: " << filepath << std::endl;

    char timestampStr[20];
    strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%d %H:%M:%S", &t);
    return std::string(timestampStr);
}

std::string saveCapturedFaceFrame(const cv::Mat& frame, const std::tm& t) {
    char filename[100];
    strftime(filename, sizeof(filename), "capture_%Y%m%d_%H%M%S.jpg", &t);
    std::string filepath = "E:/SmartAttendance/SmartAttendance/captures/" + std::string(filename);
    imwrite(filepath, frame);
    std::cout << "✅ Saved captured frame: " << filepath << std::endl;

    char timestampStr[20];
    strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%d %H:%M:%S", &t);
    return std::string(timestampStr);
}

void captureAndSaveFace(Mat& frame, const std::string& personName, int imgCount) {
    static int savedCount = 0;
    string personDir = "E:/SmartAttendance/SmartAttendance/images/" + personName;

    if (!filesystem::exists(personDir)) {
        filesystem::create_directory(personDir);
    }

    vector<Rect> faces;
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, Size(30, 30));

    // Save only the first detected face
    if (!faces.empty() && savedCount < imgCount) {
        Rect face = faces[0];
        Mat faceROI = gray(face);
        resize(faceROI, faceROI, Size(200, 200));

        string filename = personDir + "/" + to_string(savedCount) + ".jpg";
        imwrite(filename, faceROI);
        cout << "Saved image: " << filename << endl;
        savedCount++;
    }

    if (savedCount >= imgCount) {
        cout << "Image capture complete for: " << personName << endl;
        savedCount = 0;
    }
}


cv::Rect getLastDetectedFaceROI() {
    return lastDetectedFace;
}