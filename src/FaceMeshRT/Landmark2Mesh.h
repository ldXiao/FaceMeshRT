//
// Created by Lind Xiao on 11/30/18.
//

#ifndef FACEMESHRT_LANDMARK2MESH_H
#define FACEMESHRT_LANDMARK2MESH_H

#endif //FACEMESHRT_LANDMARK2MESH_H

#include <Eigen/Core>
#include <opencv2/core.hpp>
namespace FaceMeshRT{
    int MeshGen();
    bool MeshUpdate(Eigen::MatrixXd* meshigl, cv::Mat frame, Eigen::Matrix<double, 68,2> landmark2d);
}