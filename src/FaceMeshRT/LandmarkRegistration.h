//
// Created by Lind Xiao on 12/2/18.
//

#ifndef FACEMESHRT_LANDMARKREGISTRATION_H
#define FACEMESHRT_LANDMARKREGISTRATION_H

#endif //FACEMESHRT_LANDMARKREGISTRATION_H

#include <Eigen/Core>

namespace FaceMeshRT{
    bool LandmarkRegistration1(Eigen::MatrixXd* landmark3d, Eigen::MatrixXd mesh);
    bool LandmarkRegistration2(Eigen::MatrixXd* landmark3d, Eigen::MatrixXd mesh);
}