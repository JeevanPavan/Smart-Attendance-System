// ===== train_faces.cpp =====
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

namespace fs = std::filesystem;
using namespace cv;
using namespace cv::face;
using std::string; using std::vector; using std::map;

static const string MODEL_DIR = "E:/SmartAttendance/SmartAttendance/models/";
static const string IMAGE_DIR = "E:/SmartAttendance/SmartAttendance/images/";

int main() {
    vector<Mat> images;
    vector<int> labels;
    map<int, string> labelToName;

    int label = 1;  // ✅ Start from 1, not 0
    for (const auto& person : fs::directory_iterator(IMAGE_DIR)) {
        if (!person.is_directory()) continue;
        string name = person.path().filename().string();
        labelToName[label] = name;

        for (const auto& img : fs::directory_iterator(person.path())) {
            Mat m = imread(img.path().string(), IMREAD_GRAYSCALE);
            if (!m.empty()) {
                resize(m, m, Size(200, 200));
                images.push_back(m);
                labels.push_back(label);
            }
        }
        ++label;
    }

    if (images.empty()) {
        std::cerr << "❌ No images found for training.\n";
        return -1;
    }

    auto model = LBPHFaceRecognizer::create();
    model->train(images, labels);
    model->save(MODEL_DIR + "face_model.yml");
    std::cout << "✅ Model trained and saved.\n";

    std::ofstream mapFile(MODEL_DIR + "label_map.txt");
    for (auto& [id, name] : labelToName)
        mapFile << id << ' ' << name << '\n';
    std::cout << "✅ Label map saved.\n";

    return 0;
}
