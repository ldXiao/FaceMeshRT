//
// Created by Lind Xiao on 11/30/18.
//
#include "Face2Landmark.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <Eigen/Core>

using namespace std;




namespace FaceMeshRT {
    void LandmarkWrite(dlib::full_object_detection shape, string);

    void Calibration(cv::VideoCapture cap) {
        int photocount = 0;
        dlib::shape_predictor pose_model;
        dlib::deserialize(
                "/Users/vector_cat/gits/FaceMeshRT/data/shape_predictor_68_face_landmarks.dat"
        ) >> pose_model;
        // Use a 68 point predictor trained in iBug.
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        cout << "[INFO] loading facial landmark predictor..." << endl;
        cout << "[INFO] press 'q'  to quite from test mode"<<endl;
        cout << "[INFO] press 's' to shot a photo to generate mesh"<<endl;
        for (;;) {
            cv::Mat frame, grey_frame;
            std::string text;
            // import a frame from the dual lense camera, which consists of two parts
            cap >> frame;
            // cut into left frame and right frame and present after resizing
            cv::Rect rect_left(0,0, frame.cols/2, frame.rows);
            cv::Rect rect_right(frame.cols/2, 0, frame.cols/2, frame.rows);
            cv::Mat frame_left = frame(rect_left);
            cv::Mat frame_right = frame(rect_right);
            cv::resize(frame_left, frame_left, cv::Size(frame_left.cols * 0.5, frame_left.rows * 0.5), 0, 0, CV_INTER_LINEAR);
            cv::resize(frame_right, frame_right, cv::Size(frame_right.cols * 0.5, frame_right.rows * 0.5), 0, 0, CV_INTER_LINEAR);
            cv::cvtColor(frame_left, grey_frame, CV_BGR2GRAY);
            dlib::cv_image<unsigned char> dframe(grey_frame);
            // import a frame from  the video input and transform it into something dlib can understand
            // work on some thing smaller and grey so as to speed up
            // display the smaller bgr one;
            std::vector<dlib::rectangle> faces = detector(dframe);
            //the detector automatically give an array of detected faces
            char keyboard_input = cv::waitKey(25);
            if (faces.size() > 0) {
                text = std::to_string(faces.size()) + "face(s) found," + std::to_string(photocount) + "photo(s) taken";
                cv::putText(frame_left, text, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);

                dlib::full_object_detection shape = pose_model(dframe, faces[0]);
                // detect the land scape on the first face.
                if (keyboard_input == 's') {
                    // shot a face photo
                    photocount += 1;
                    LandmarkWrite(
                            shape,
                            "/Users/vector_cat/gits/FaceMeshRT/data/face"
                            + std::to_string(photocount) + ".pts"
                    );
                    try {
                        int cx = (faces[0].left() + faces[0].right()) / 2;
                        int cy = (faces[0].bottom() + faces[0].top()) / 2;
                        cv::Rect roi(cx - 100, cy - 120, 200, 240);
                        cv::Mat corped = grey_frame(roi);
                        cv::imshow("corped", corped);
                        std::string photo_directory =
                                "/Users/vector_cat/CLionProjects/FaceMeshRT/data/face" + std::to_string(photocount) +
                                ".png";
                        cv::imwrite(photo_directory, frame_left);
                    }
                    catch (exception &e) {
                        cout << "Photo shot failed, please adjust your "
                             << "gesture so that the green rectangle is inside the screen"
                             << endl;
                    }

                }
            }
            for (auto it = faces.begin(); it != faces.end(); ++it) {
                // compute the bounding box of faces
                long cx = ((*it).left() + (*it).right()) / 2;
                long cy = ((*it).top() + (*it).bottom()) / 2;
                cv::rectangle(frame_left, cv::Point(cx - 100, cy - 120), cv::Point(cx + 100, cy + 120),
                              cv::Scalar(0, 255, 0), 1);
                //draw a rectangle on the frame
            }
            cv::imshow("Frame_left", frame_left);
            cv::imshow("Frame_right", frame_right);
            if (keyboard_input == 'q') {
                // release the camera
                break;
            }
        }
        return;
    }


    void LandmarkWrite(dlib::full_object_detection shape, string pts_dir) {
        cout << "[INFO] writting landmark file" << endl;
        ofstream outstream;
        outstream.open(pts_dir, fstream::out | fstream::trunc);
        // this  is the standard head of 68 point landmark pts file
        outstream << "version: 1\n";
        outstream << "n_points:  68\n";
        outstream << "{\n";
        for (int ii = 0; ii < shape.num_parts(); ii++) {
            outstream << (shape.part(ii)).x() << " " << (shape.part(ii)).y() << "\n";
            // dlib::vector does not have [] operator or at()
        }
        outstream << "}";
        outstream.close();
        return;
    }
    bool Update2DLandmark(Eigen::Matrix<double, 68,2> *landmark2d, cv::Mat frame, dlib::shape_predictor pose_model) {
        cv::Rect rect_left(0,0, frame.cols/2, frame.rows);
        cv::Mat frame_left = frame(rect_left);
        cv::resize(frame_left, frame_left, cv::Size(frame_left.cols * 0.5, frame_left.rows * 0.5), 0, 0, CV_INTER_LINEAR);
        cv::Mat grey_frame_left;
        cv::cvtColor(frame_left, grey_frame_left, CV_BGR2GRAY);
        dlib::cv_image<unsigned char> dframe_left(grey_frame_left);
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        std::vector<dlib::rectangle> faces_left = detector(dframe_left);
        if(faces_left.size()==1){
            dlib::full_object_detection shape_left = pose_model(dframe_left, faces_left[0]);
            for(int ii=0; ii<shape_left.num_parts(); ii++){
                (*landmark2d)(ii, 0)= (shape_left.part(ii)).x();
                (*landmark2d)(ii, 1)= (shape_left.part(ii)).y();
            }
            return true;
        }

        return false;
    }

    void Showimage_Dual_Lence(cv::Mat frame, Eigen::Matrix<double, 68, 2> landmark2d){
        cv::Rect rect_left(0,0, frame.cols/2, frame.rows);
        cv::Mat frame_left = frame(rect_left);
        cv::resize(frame_left, frame_left, cv::Size(frame_left.cols * 0.5, frame_left.rows * 0.5), 0, 0, CV_INTER_LINEAR);
        for(int ii=1; ii<=68; ii++) {
            int x = landmark2d(ii,0);
            int y = landmark2d(ii,1);
            cv::circle(frame_left, cv::Point(x, y), 1, (0, 0, 255), -1);
            cv::putText(frame_left, std::to_string(ii), cv::Point(x-10, y-10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
        }
        cv::imshow("Face",frame_left);
    }

    bool Update3DLandmark(Eigen::MatrixXd* landmark3d, cv::Mat frame, dlib::shape_predictor pose_model){
        // A function used to update the real-time 3d landmarks
        cv::Rect rect_left(0,0, frame.cols/2, frame.rows);
        cv::Rect rect_right(frame.cols/2, 0, frame.cols/2, frame.rows);
        cv::Mat frame_left = frame(rect_left);
        cv::Mat frame_right = frame(rect_right);
        cv::resize(frame_left, frame_left, cv::Size(frame_left.cols * 0.5, frame_left.rows * 0.5), 0, 0, CV_INTER_LINEAR);
        cv::resize(frame_right, frame_right, cv::Size(frame_right.cols * 0.5, frame_right.rows * 0.5), 0, 0, CV_INTER_LINEAR);
        cv::Mat grey_frame_left, grey_frame_right;
        cv::cvtColor(frame_left, grey_frame_left, CV_BGR2GRAY);
        cv::cvtColor(frame_right, grey_frame_right, CV_BGR2GRAY);
        dlib::cv_image<unsigned char> dframe_left(grey_frame_left);
        dlib::cv_image<unsigned char> dframe_right(grey_frame_right);
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        std::vector<dlib::rectangle> faces_left = detector(dframe_left);
        std::vector<dlib::rectangle> faces_right = detector(dframe_right);
        if(faces_left.size()==1 && faces_right.size()==1){
            cout<<"true"<<endl;
            dlib::full_object_detection shape_left = pose_model(dframe_left, faces_left[0]);
            dlib::full_object_detection shape_right = pose_model(dframe_right, faces_right[0]);
            double f = 335.5; //focal length specific to my camera
            double B = 60; // base line of the dual lence
            for(int ii=0; ii<shape_left.num_parts(); ii++){
                // calculate the depth from the camera data;
                double z = (B * f)/((shape_left.part(ii)).x()-(shape_right.part(ii)).x());
                (*landmark3d)(ii, 2)= -z;
                (*landmark3d)(ii, 0)= ((shape_left.part(ii)).x()- frame_left.cols/2)* z /f;
                (*landmark3d)(ii, 1)= ((shape_left.part(ii)).y()- frame_left.rows/2)* z /f;
            }
            cout<<(*landmark3d)<<endl;
            return true;
        }

        return false;
    }
}