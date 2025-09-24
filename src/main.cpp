// ===== main.cpp =====
#include <opencv2/opencv.hpp>
#include "face_detect.h"
#include "db_helper.h"
#include "ui.h"
#include <windows.h>
#include <fstream>
#include <map>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

tm getCurrentLocalTime() {
    time_t now = time(0);
    std::tm t;
    localtime_s(&t, &now);
    return t;
}

static map<int, string> loadLabelMap(const string& path) {
    map<int, string> m;
    ifstream in(path);
    int id; string name;
    while (in >> id >> name) m[id] = name;
    return m;
}

int main(int argc, char* argv[]) {
    string mode = "attendance";
    string newName = "";

    if (argc > 1) {
        mode = argv[1];
        if (mode == "register" && argc > 2) {
            newName = argv[2];
        }
    }

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "❌ Camera not accessible!" << endl;
        return -1;
    }

    loadFaceModel("E:/SmartAttendance/SmartAttendance/models/face_model.yml");
    map<int, string> labelMap =
        loadLabelMap("E:/SmartAttendance/SmartAttendance/models/label_map.txt");

    CascadeClassifier eye_cascade;
    if (!eye_cascade.load("E:/SmartAttendance/SmartAttendance/models/haarcascade_eye.xml")) {
        cerr << "❌ Failed to load haarcascade_eye.xml" << endl;
        return -1;
    }

    Mat frame;

    if (mode == "register") {
        if (newName.empty()) {
            cerr << "Please provide a name for registration." << endl;
            return -1;
        }

        cout << "Registering new user: " << newName << endl;
        for (int i = 0; i < 10; ++i) {
            cap >> frame;
            if (frame.empty()) break;
            captureAndSaveFace(frame, newName, 10);
            imshow("Registering: " + newName, frame);
            waitKey(1000);
        }

        cout << "✅ Face data saved. Run train_faces.exe to update model." << endl;
        cap.release();
        destroyAllWindows();
        return 0;
    }

    bool attendanceMarked = false;
    int unknownCaptured = 0;
    const int UNKNOWN_LIMIT = 3;

    while (!attendanceMarked && unknownCaptured < UNKNOWN_LIMIT) {
        cap >> frame;
        if (frame.empty()) break;

        std::vector<std::tuple<int, double, Rect>> faceResults;
        bool detected = detectAndRecognizeFace(frame, faceResults);

        std::tm nowTime = getCurrentLocalTime();
        char timestampStr[20];
        strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%d %H:%M:%S", &nowTime);
        std::string timestamp(timestampStr);

        if (detected && !faceResults.empty()) {
            auto [predicted_label, confidence, faceROI] = faceResults[0];

            cout << "🔍 Prediction - ID: " << predicted_label << ", Confidence: " << confidence << endl;

            Mat face = frame(faceROI);
            Mat gray;
            cvtColor(face, gray, COLOR_BGR2GRAY);

            vector<Rect> eyes;
            eye_cascade.detectMultiScale(gray, eyes);
            cout << "👁️ Eyes detected: " << eyes.size() << endl;

            bool isLive = eyes.size() >= 1;  // ✅ Less strict for better detection

            if (confidence < 80 && isLive) {  // ✅ Slightly relaxed threshold
                string studentName =
                    labelMap.count(predicted_label)
                    ? labelMap[predicted_label]
                    : "Unknown";

                drawLabel(frame, "Name: " + studentName + "  (ID " + to_string(predicted_label) + ')',
                    Scalar(0, 255, 0));

                saveCapturedFaceFrame(frame, nowTime);
                markAttendance(predicted_label, studentName, timestamp);

                Beep(1000, 150);
                attendanceMarked = true;
            }
            else if (confidence < 80 && !isLive) {
                drawLabel(frame, "⚠️ Spoof?", Scalar(0, 0, 255));
            }
            else {
                drawLabel(frame, "Unknown", Scalar(0, 0, 255));
                string unknownTime = saveUnknownFace(face, nowTime);
                markAttendance(0, "Unknown", unknownTime);
                ++unknownCaptured;
            }
        }

        int count = getTodaysCount();
        displayLiveCount(frame, count);

        imshow("Smart Attendance", frame);
        if (waitKey(1) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
