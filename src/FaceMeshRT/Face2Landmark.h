//
// Created by Lind Xiao on 11/30/18.
//

#ifndef FACEMESHRT_FACE2MESH_H
#define FACEMESHRT_FACE2MESH_H

#endif //FACEMESHRT_FACE2MESH_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <thread>
#include <chrono>
#include <Eigen/Core>
#include <fstream>

namespace FaceMeshRT{
    void LandmarkWrite(dlib::full_object_detection shape, std::string);
    void Calibration(cv::VideoCapture cap);
    bool Update3DLandmark(Eigen::MatrixXd* landmark3d, cv::Mat frame, dlib::shape_predictor pose_model);
    bool Update2DLandmark(Eigen::Matrix<double, 68, 2>* landmark2d, cv::Mat frame, dlib::shape_predictor pose_model);
    void Showimage_Dual_Lence(cv::Mat frame, Eigen::Matrix<double, 68, 2> landmark2d);
}