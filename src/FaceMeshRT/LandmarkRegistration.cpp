//
// Created by Lind Xiao on 12/2/18.
//

#include "LandmarkRegistration.h"
#include <Eigen/Dense>
#include <iostream>
using namespace std;
using namespace Eigen;
namespace FaceMeshRT{
    bool LandmarkRegistration1(Eigen::MatrixXd* landmark3d, Eigen::MatrixXd mesh){
        // a basic assumption is that the real-time face is related to the landmarks on
        // mesh by an affine transformation, we need to firstly transform the physics-space landmark3d
        // near the mesh landmarks and
        // then use the transformed landmarks as handles for deformation of the mesh
        // (but dealt in another function)

        // There are 12 parameters in the affine transformation,
        // we need to find 4 points among the landmarks in order to solve for the matrix.
        // A good choice is to choose the relatively "fixed" points such that are not affected by expression
        // We choose the inner corners of two eyes,  nose tip and the nose middle.
        // their indices corresponding to both ibug-landmark file and sfm3448-face mesh is listed is file
        // "../data/ibug_to_sfm.txt"


        // left eye inner corner
        Eigen::RowVector4d x0((*landmark3d)(43, 0), (*landmark3d)(43,1), (*landmark3d)(43,2),1);
        Eigen::RowVector4d y0(mesh(614, 0), mesh(614,1), mesh(614,2),1);
        //right eye inner corner
        Eigen::RowVector4d x1((*landmark3d)(40, 0), (*landmark3d)(40,1), (*landmark3d)(40,2),1);
        Eigen::RowVector4d y1(mesh(181, 0), mesh(181,1), mesh(181,2),1);
        //nose middle
        Eigen::RowVector4d x2((*landmark3d)(29, 0), (*landmark3d)(29,1), (*landmark3d)(29,2),1);
        Eigen::RowVector4d y2(mesh(379, 0), mesh(379,1), mesh(379,2),1);
        //nose tip
        Eigen::RowVector4d x3((*landmark3d)(31, 0), (*landmark3d)(31,1), (*landmark3d)(31,2),1);
        Eigen::RowVector4d y3(mesh(114, 0), mesh(114,1), mesh(114,2),1);

        // chin
        Eigen::RowVector4d x4((*landmark3d)(9, 0), (*landmark3d)(9,1), (*landmark3d)(9,2),1);
        Eigen::RowVector4d y4(mesh(33, 0), mesh(33,1), mesh(33,2),1);




        Eigen::RowVector4d x5((*landmark3d)(58, 0), (*landmark3d)(58,1), (*landmark3d)(58,2),1);
        Eigen::RowVector4d y5(mesh(411, 0), mesh(411,1), mesh(411,2),1);
        // Ax+b =y, we solve for A, b as a 4*4 matrix
        // for convenience, we initialized as row vectors and we need to solve
        // X * Ab = Y for Ab

        Eigen::Matrix4d X, Y, Ab;
        X << x0, x1, x2, x3, x4, x5;
        Y << y0, y1, y2, y3, y4, y5;
        Ab = X.bdcSvd(ComputeThinU | ComputeThinV).solve(Y);
        if(abs(X.determinant())<0.00001){
            cout <<"[Causion]linear equation too singular"<<endl;
            return false;
        }

        // we transform landmark
        for(int ii =0; ii< 68; ii+=1){
            Eigen::RowVector4d x((*landmark3d)(ii, 0), (*landmark3d)(ii,1), (*landmark3d)(ii,2),1);
            for(int jj = 0; jj<3; jj++){
                (*landmark3d)(ii, jj) = (x * Ab)(jj);
            }
        }
        cout<< Ab <<endl;
        return true;
    }
}
