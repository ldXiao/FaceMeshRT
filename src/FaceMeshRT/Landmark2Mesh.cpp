//
// Created by Lind Xiao on 11/30/18.
//

#include "Landmark2Mesh.h"
#include <eos/core/Image.hpp>
#include <eos/core/image/opencv_interop.hpp>
#include <eos/core/Landmark.hpp>
#include <eos/core/LandmarkMapper.hpp>
#include <eos/core/read_pts_landmarks.hpp>
#include <eos/fitting/fitting.hpp>
#include <eos/morphablemodel/Blendshape.hpp>
#include <eos/morphablemodel/MorphableModel.hpp>
#include <eos/render/draw_utils.hpp>
#include <eos/render/texture_extraction.hpp>
#include <eos/cpp17/optional.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <Eigen/Core>

#include <iostream>
#include <string>
#include <vector>
using namespace eos;
using eos::core::Landmark;
using eos::core::LandmarkCollection;
using cv::Mat;
using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace FaceMeshRT {
    int MeshGen() {
        //set the file dirs
        string data_dir = "/Users/vector_cat/gits/FaceMeshRT/data/";
        string landmarksfile = data_dir + "face1.pts";
        string imagefile = data_dir + "face1.png";
        string modelfile = data_dir + "sfm_shape_3448.bin";
        string mappingsfile = data_dir + "ibug_to_sfm.txt";
        string edgetopologyfile = data_dir + "sfm_3448_edge_topology.json";
        string blendshapesfile = data_dir + "expression_blendshapes_3448.bin";
        string contourfile = data_dir + "sfm_model_contours.json";

        //this piece of code is a copy of the example fit-model in eos project
        // see https://github.com/patrikhuber/eos/blob/master/examples/fit-model.cpp
        Mat image = cv::imread(imagefile);
        LandmarkCollection<Eigen::Vector2f> landmarks;
        try {
            landmarks = core::read_pts_landmarks(landmarksfile);
        } catch (const std::runtime_error &e) {
            cout << "Error reading the landmarks: " << e.what() << endl;
            return EXIT_FAILURE;
        }
        morphablemodel::MorphableModel morphable_model;
        try {
            morphable_model = morphablemodel::load_model(modelfile);
        } catch (const std::runtime_error &e) {
            cout << "Error loading the Morphable Model: " << e.what() << endl;
            return EXIT_FAILURE;
        }
        // The landmark mapper is used to map 2D landmark points (e.g. from the ibug scheme) to vertex ids:
        core::LandmarkMapper landmark_mapper;
        try {
            landmark_mapper = core::LandmarkMapper(mappingsfile);
        } catch (const std::exception &e) {
            cout << "Error loading the landmark mappings: " << e.what() << endl;
            return EXIT_FAILURE;
        }

        // The expression blendshapes:
        const vector<morphablemodel::Blendshape> blendshapes = morphablemodel::load_blendshapes(blendshapesfile);

        morphablemodel::MorphableModel morphable_model_with_expressions(
                morphable_model.get_shape_model(), blendshapes, morphable_model.get_color_model(), cpp17::nullopt,
                morphable_model.get_texture_coordinates());

        // These two are used to fit the front-facing contour to the ibug contour landmarks:
        const fitting::ModelContour model_contour =
                contourfile.empty() ? fitting::ModelContour() : fitting::ModelContour::load(contourfile);
        const fitting::ContourLandmarks ibug_contour = fitting::ContourLandmarks::load(mappingsfile);

        // The edge topology is used to speed up computation of the occluding face contour fitting:
        const morphablemodel::EdgeTopology edge_topology = morphablemodel::load_edge_topology(edgetopologyfile);


        // Fit the model, get back a mesh and the pose:
        core::Mesh mesh;
        fitting::RenderingParameters rendering_params;
        std::tie(mesh, rendering_params) = fitting::fit_shape_and_pose(
                morphable_model_with_expressions, landmarks, landmark_mapper, image.cols, image.rows, edge_topology,
                ibug_contour, model_contour, 5, cpp17::nullopt, 30.0f);

        // The 3D head pose can be recovered as follows:
        float yaw_angle = glm::degrees(glm::yaw(rendering_params.get_rotation()));
        // and similarly for pitch and roll.

        // Extract the texture from the image using given mesh and camera parameters:
        const Eigen::Matrix<float, 3, 4> affine_from_ortho =
                fitting::get_3x4_affine_camera_matrix(rendering_params, image.cols, image.rows);
        //For some reason, the below line will case building failure for mac OS, we simply abandoned it
//        const core::Image4u isomap =
//                render::extract_texture(mesh, affine_from_ortho, core::from_mat(image), true);


        // Save the mesh as textured obj:
        core::write_textured_obj(mesh, (data_dir + "face_mesh.obj"));


//    // And save the isomap:
//        cv::imwrite((data_dir + "face.isomap.png"), core::to_mat(isomap));

        cout << "Finished fitting and wrote result mesh to files"
             << endl;

    }

    LandmarkCollection<Eigen::Vector2f> Eigen_to_eos_landmark(Eigen::MatrixXd landmark2d){
        LandmarkCollection<Eigen::Vector2f> landmarks;
        landmarks.reserve(68);
        for(int ii=1; ii<=68; ii++){
            Landmark<Eigen::Vector2f> landmark;
            landmark.name=std::to_string(ii);
            landmark.coordinates[0] = landmark2d(ii, 0)-1.0f;
            landmark.coordinates[1] = landmark2d(ii, 1)-1.0f;
            landmarks.emplace_back(landmark);
        }
        return landmarks;

    }

    bool MeshUpdate(Eigen::MatrixXd* meshigl, cv::Mat frame, Eigen::Matrix<double, 68,2> landmark2d){
        string data_dir = "/Users/vector_cat/CLionProjects/Face_Mesh/data/";
        string modelfile = data_dir + "sfm_shape_3448.bin";
        string mappingsfile = data_dir + "ibug_to_sfm.txt";
        string edgetopologyfile = data_dir + "sfm_3448_edge_topology.json";
        string blendshapesfile = data_dir + "expression_blendshapes_3448.bin";
        string contourfile = data_dir + "sfm_model_contours.json";
        // we have to implicitly convert double to float here
        LandmarkCollection<Eigen::Vector2f> landmarks = Eigen_to_eos_landmark(landmark2d);
        morphablemodel::MorphableModel morphable_model;
        try {
            morphable_model = morphablemodel::load_model(modelfile);
        } catch (const std::runtime_error &e) {
            cout << "Error loading the Morphable Model: " << e.what() << endl;
            return EXIT_FAILURE;
        }
        // The landmark mapper is used to map 2D landmark points (e.g. from the ibug scheme) to vertex ids:
        core::LandmarkMapper landmark_mapper;
        try {
            landmark_mapper = core::LandmarkMapper(mappingsfile);
        } catch (const std::exception &e) {
            cout << "Error loading the landmark mappings: " << e.what() << endl;
            return EXIT_FAILURE;
        }
        // The expression blendshapes:
        const vector<morphablemodel::Blendshape> blendshapes = morphablemodel::load_blendshapes(blendshapesfile);

        morphablemodel::MorphableModel morphable_model_with_expressions(
                morphable_model.get_shape_model(), blendshapes, morphable_model.get_color_model(), cpp17::nullopt,
                morphable_model.get_texture_coordinates());

        // These two are used to fit the front-facing contour to the ibug contour landmarks:
        const fitting::ModelContour model_contour =
                contourfile.empty() ? fitting::ModelContour() : fitting::ModelContour::load(contourfile);
        const fitting::ContourLandmarks ibug_contour = fitting::ContourLandmarks::load(mappingsfile);

        // The edge topology is used to speed up computation of the occluding face contour fitting:
        const morphablemodel::EdgeTopology edge_topology = morphablemodel::load_edge_topology(edgetopologyfile);

        core::Mesh mesheos;
        fitting::RenderingParameters rendering_params;
        std::tie(mesheos, rendering_params) = fitting::fit_shape_and_pose(
                morphable_model_with_expressions, landmarks, landmark_mapper, frame.cols, frame.rows, edge_topology,
                ibug_contour, model_contour, 5, cpp17::nullopt, 30.0f);
        for(int ii=0; ii < (*meshigl).rows(); ii++){
            for(int jj=0; jj<3; jj++){
                (*meshigl)(ii, jj) = mesheos.vertices[ii][jj];
            }
        }
        return true;
    }
}
