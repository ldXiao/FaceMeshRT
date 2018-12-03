#include <igl/colon.h>
#include <igl/harmonic.h>
#include <igl/readOBJ.h>
#include <igl/readDMAT.h>
#include <igl/opengl/glfw/Viewer.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include "FaceMeshRT/Face2Landmark.h"
#include "FaceMeshRT/Landmark2Mesh.h"
#include "FaceMeshRT/LandmarkRegistration.h"
using namespace FaceMeshRT;
using namespace std;

const Eigen::RowVector3d sea_green(70./255.,252./255.,167./255.);
const Eigen::RowVector3d red(170./255.,30./255.,30./255.);

Eigen::MatrixXd V;
Eigen::MatrixXi F;
Eigen::Matrix<double, 68,2> landmark2d;
cv::Mat frame;
dlib::shape_predictor pose_model;
cv::VideoCapture cap1;
void loadshape_predictor(){
    dlib::deserialize(
            "/Users/vector_cat/gits/FaceMeshRT/data/shape_predictor_68_face_landmarks.dat"
    ) >> pose_model;
    cout<<"[INFO] initializing shape predictor"<<endl;
}

bool pre_draw(igl::opengl::glfw::Viewer &viewer){
    cap1>> frame;
    Update2DLandmark(&landmark2d, frame, pose_model);
    Showimage_Dual_Lence(frame, landmark2d);
    viewer.data().set_vertices(V);
    return false;
}

bool key_down(igl::opengl::glfw::Viewer &viewer,  unsigned char key, int mods){
    cout<<"called"<<endl;
    switch(key){
        case ' ':
            cout<<"stoped"<<endl;
            viewer.core.is_animating = !viewer.core.is_animating;
            break;
        case ',':
            auto start = std::chrono::high_resolution_clock::now();
            if(MeshUpdate(&V, frame, landmark2d)) {
                viewer.data().set_vertices(V);
            }
            auto stop = std::chrono::high_resolution_clock::now();
            cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() <<"millisecond"<<endl;
            return true;
    }
    return false;
}


int main(int argc, char *argv[])
{
    using namespace Eigen;
    loadshape_predictor();
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "[Err] Can not open video stream" << std::endl;
        return -1;
    }
    cout << "[INFO] camera sensor warming up" << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // wait two seconds to warm up the sensor.
    Calibration(cap);

    MeshGen();

    cap.release();
    if (!cap.isOpened()) {
        std::cout << "cmera is released" << std::endl;
    }
    cout<<"Your face_mesh has been created"<<endl;


    // Initialize landmark2d;
    landmark2d = Eigen::MatrixXd::Zero(68,2);
    // V, F are initialized here;
    const std::string dir="/Users/vector_cat/gits/FaceMeshRT/data/face_mesh.obj";
    igl::readOBJ(dir,V,F);

    //transform landtest;
    // Plot the mesh
    cap1 = cv::VideoCapture(0);
    if (!cap1.isOpened()) {
        std::cout << "[Err] Can not open video stream" << std::endl;
        return -1;
    }

    igl::opengl::glfw::Viewer viewer;
    Eigen::Vector4i b(33, 181, 379, 114);
    viewer.data().add_points(igl::slice(V, b, 1), red);
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    //viewer.core.trackball_angle = Eigen::Quaternionf(0.81,0.58,0.03,0.03);
    viewer.core.trackball_angle.normalize();
    viewer.callback_pre_draw = &pre_draw;
    viewer.callback_key_down = &key_down;
    viewer.core.is_animating = true;
    viewer.core.animation_max_fps = 30.;
    viewer.launch();
}
