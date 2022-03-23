// Copyright (c) 2021 Matthew Elks

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include"./doctest/doctest.h"
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include"../libs/agp/agp.h"
#include<iostream>
#include<string>
#include<sstream>
#include<cmath>


TEST_CASE("MultiplyModelViewProj()"){
    float mat4_1[16] = {
        1.34, 2.29, 3.21, 4.9,
        3.33, 2.29, 1.135, 9.56,
        4.78, 3.98, 2.11, 5.65,
        1.11, 9.89, 5.12, 3.39};
    float mat4_2[16] = {4, 6, 4, 3, 5, 5, 6, 7, 6, 4, 5, 4, 7, 3, 9, 0};
    float expect[16] = {
        46.64, 71.71, 52.85, 45.14,
        60.28, 101.83, 73.055, 40.395,
        68.39, 96.01, 77.75, 64.39,
        94.43, 109.45, 85.38, 83.52
    };

    float mat4_result[16];
    Mat4MultiplyMat4T(mat4_1, mat4_2, mat4_result);

    for(int i=0;i<16;i++){
        CHECK(mat4_result[i] == doctest::Approx( expect[i] ).epsilon(0.0001));
    }
}


TEST_CASE("TransposeMat4()"){

    float mat4[16] = {
        1.34, 2.29, 3.21, 4.9,
        3.33, 2.29, 1.135, 9.56,
        4.78, 3.98, 2.11, 5.65,
        1.11, 9.89, 5.12, 3.39
    };

    float mat4_trans[16];

    float expect[16] = {
        1.34, 3.33, 4.78, 1.11,
        2.29, 2.29, 3.98, 9.89,
        3.21, 1.135, 2.11, 5.12,
        4.9, 9.56, 5.65, 3.39
    };

    TransposeMat4(mat4, mat4_trans);

    for(int i=0;i<16;i++){
        CHECK(mat4_trans[i] == doctest::Approx( expect[i] ).epsilon(0.0001));
    }
}


TEST_CASE("CrossVec3()"){

    float epsilon = 0.000001;

    float vec_1[3] = {0.8,3.9,2.1};
    float vec_2[3] = {1.5,3.3,1.2};
    float vec[3];
    float expect[3] = {-2.25, 2.19, -3.21};

    CrossVec(vec_1, vec_2, vec);

    for(int i=0;i<3;i++){
        REQUIRE(abs(expect[i] - vec[i]) < epsilon);
    }
}


TEST_CASE("NormalizeVec3()"){

    float epsilon = 0.000001;

    float vec[3] = {-2.25, 2.19, -3.21};
    float expect[3] = {-0.501081, 0.487719, -0.714876};

    NormalizeVec<3>(vec);

    for(int i=0;i<3;i++){
        REQUIRE(abs(expect[i] - vec[i]) < epsilon);
    }

}


TEST_CASE("DiffVec3()"){
    float vec1[3] = { 5, -1.2, 0.0005};
    float vec2[3] = {-1.1, 7.1, 8.973};
    float output[3];

    float expect[3] = {6.1, -8.3, -8.9725};

    DiffVec<3>(vec1, vec2, output);

    for(int i=0;i<3;i++){
        CHECK(output[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
    }
}

TEST_CASE("MagnitudeVec3()"){
    float vec1[3] = { 5, -1.2, 0.0005};

    float expect = 5.14198;

    float output = MagnitudeVec<3>(vec1);

    CHECK(output == doctest::Approx( expect ).epsilon(0.000001));

}


TEST_CASE("Arcball"){

    auto set_arc_vars_functor = [&](arcball &arc){
        static float camera_position[3] = {1.41, 2.05, 4.39};
        static float up_vec[3] = {0, 0, 1};
        static float center_position[3] = {0.3, 1.5, 0.083};
        static float fov = 40*3.14/180;
        static float z_near = 0.1;
        static float z_far = 10.95;
        static float window_width = 1600;
        static float window_height = 900;
        arc.rotate_sensitivity = 0.01;
        arc.zoom_sensitivity = 0.9;
        arc.SetViewArea(window_width, window_height);
        arc.SetProjectionVars(fov, z_near, z_far);
        arc.SetCamera(camera_position, up_vec);
        arc.SetCenter(center_position);
    };

    SUBCASE("ViewProjMatrix() && SetViewArea() && SetProjectionVars() && SetCamera() && arc.SetCenter();"){
        arcball arc;
        set_arc_vars_functor(arc);

        float expect[16] = { -1.201332, 0.955337, 0.187612, -1.088178,
                             -1.355309, -1.970485, 1.355462, -0.000000,
                             -0.252244, -0.124986, -0.978753, 4.706768,
                             -0.247679, -0.122724, -0.961038, 4.819767 };

        float value[16];
        arc.ViewProjMatrix(value);

        for(int i=0;i<16;i++){
            CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("Rotate()"){
        float delta_x = 4.4;
        float delta_y = 5.3;
        arcball arc;
        set_arc_vars_functor(arc);

        float expect[16] = { -1.175853, 0.972757, 0.249360, -1.127076,
                             -1.386975, -1.987564, 1.213262, 0.067032,
                             -0.256294, -0.057569, -0.983974, 4.670463,
                             -0.251656, -0.056527, -0.966165, 4.784120 };

        arc.Rotate(delta_x, delta_y);
        float value[16];
        arc.ViewProjMatrix(value);

        for(int i=0;i<16;i++){
            CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("Zoom()"){

        float mouse_x = 4.4;
        float mouse_y = 5.3;
        float zoom_level = 3.1;
        arcball arc;
        set_arc_vars_functor(arc);
        
        float expect[16] = { -1.201332, 0.955337, 0.187612, -1.088178,
                             -1.355309, -1.970485, 1.355462, -2.023154,
                             -0.252244, -0.124986, -0.978753, 7.548197,
                             -0.247679, -0.122724, -0.961038, 7.609768 };

        arc.Zoom(mouse_x, mouse_y, zoom_level);
        float value[16];
        arc.ViewProjMatrix(value);

        for(int i=0;i<16;i++){
            CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("Translate()"){
        float mouse_x = 4.4;
        float mouse_y = 5.3;
        arcball arc;
        set_arc_vars_functor(arc);
        
        float expect[16] = { -1.201332, 0.955337, 0.187612, -1.063529,
                             -1.355309, -1.970485, 1.355462, -0.052783,
                             -0.252244, -0.124986, -0.978753, 4.711926,
                             -0.247679, -0.122724, -0.961038, 4.824832 };

        arc.Translate(mouse_x, mouse_y);
        float value[16];
        arc.ViewProjMatrix(value);

        for(int i=0;i<16;i++){
            CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("SetCameraPos()"){
        SUBCASE("Normal Operation"){
            arcball arc;
            set_arc_vars_functor(arc);

            float expect_camera[3] = {1.41, 2.05, 4.39};

            const float *value_camera = arc.Camera();

            for(int i=0;i<3;i++){
                CHECK(value_camera[i] == doctest::Approx( expect_camera[i] ).epsilon(0.000001));
            }
        }

        SUBCASE("Aligned with up_vector"){
            float up_vec[3] = {0, 0, 1};
            float camera[3] = {0, 0, 1};
            bool is_error_thrown = false;
            arcball arc;
            set_arc_vars_functor(arc);

            float expect[16] = { -1.201332, 0.955337, 0.187612, -1.088178,
                                 -1.355309, -1.970485, 1.355462, -0.000000,
                                 -0.252244, -0.124986, -0.978753, 4.706768,
                                 -0.247679, -0.122724, -0.961038, 4.819767 };
            float expect_camera[3] = {1.41, 2.05, 4.39};

            try{
                arc.SetCamera(camera, up_vec);
            }
            catch(std::runtime_error e){
                is_error_thrown = true;
            }
            float value[16];
            arc.ViewProjMatrix(value);
            const float *value_camera = arc.Camera();

            CHECK(is_error_thrown == true);

            for(int i=0;i<16;i++){
                CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
            }

            for(int i=0;i<3;i++){
                CHECK(value_camera[i] == doctest::Approx( expect_camera[i] ).epsilon(0.000001));
            }
        
        }
    }

    SUBCASE("SetCenterPos()"){
        arcball arc;
        set_arc_vars_functor(arc);

        float expect_center[3] = {0.3, 1.5, 0.083};

        const float *value_center = arc.Center();

        for(int i=0;i<3;i++){
            CHECK(value_center[i] == doctest::Approx( expect_center[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("SetRadius()"){
        SUBCASE("Normal Operation"){
            arcball arc;
            set_arc_vars_functor(arc);
            float radius = 8.963224643;

            float expect[16] = { -1.201332, 0.955337, 0.187612, -1.088178,
                                 -1.355309, -1.970485, 1.355462, -3.249817,
                                 -0.252244, -0.124986, -0.978753, 9.270990,
                                 -0.247679, -0.122724, -0.961038, 9.301379 };
            float expect_camera[3] = {2.519999999, 2.6 , 8.697};

            arc.SetRadius(radius);

            float value[16];
            arc.ViewProjMatrix(value);
            const float *value_camera = arc.Camera();

            for(int i=0;i<16;i++){
                CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.000001));
            }

            for(int i=0;i<3;i++){
                CHECK(value_camera[i] == doctest::Approx( expect_camera[i] ).epsilon(0.000001));
            }
        }
        SUBCASE("Negative Radius"){
            arcball arc;
            set_arc_vars_functor(arc);
            float radius = -0.127;

            float expect[16] = { -1.201332, 0.955337, 0.187612, -1.088178,
                                 -1.355309, -1.970485, 1.355462, -0.000000,
                                 -0.252244, -0.124986, -0.978753, 4.706768,
                                 -0.247679, -0.122724, -0.961038, 4.819767 };
            float expect_camera[3] = {1.41, 2.05, 4.39};

            bool is_error_thrown = false;

            try{
                arc.SetRadius(radius);
            }
            catch(std::runtime_error e){
                is_error_thrown = true;
            }

            CHECK(is_error_thrown == true);

            float value[16];
            arc.ViewProjMatrix(value);
            const float *value_camera = arc.Camera();

            for(int i=0;i<16;i++){
                CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.00001));
            }

            for(int i=0;i<3;i++){
                CHECK(value_camera[i] == doctest::Approx( expect_camera[i] ).epsilon(0.000001));
            }
        }
    }

    SUBCASE("MouseRay()"){
        arcball arc;
        set_arc_vars_functor(arc);
        float mouse_x = 3.7;
        float mouse_y = 6.9;

        float expect[3] = {-0.2481, -0.12857, -0.95865};

        float value[3];
        arc.MouseRay(mouse_x, mouse_y, value);

        for(int i=0;i<3;i++){
            CHECK(value[i] == doctest::Approx( expect[i] ).epsilon(0.00001));
        }
    }

}


TEST_CASE("Quaternion"){

    SUBCASE("Operator *"){
        float epsilon = 0.000001;
        float check_quat[4] = {-0.0710933,-0.0236978,0.992673,-0.094791};

        quaternion<float> quat1 = {-1, 3, 4, 3};
        quaternion<float> quat2 = {4, 3.9, -1, -3};
        quaternion<float> quat3;

        quat3 = quat1 * quat2;

        for(int i=0;i<4;i++){
            CHECK((quat3[i] - check_quat[i])*(quat3[i] - check_quat[i]) < epsilon);
        }
    }

    SUBCASE("Operator []"){
        float epsilon = 0.000001;
        float check_quat[4] = {-0.169030851, 0.507092553, 0.676123404, 0.507092553};

        quaternion<float> quat1 = {-1, 3, 4, 3};

        for(int i=0;i<4;i++){
            CHECK((quat1[i] - check_quat[i])*(quat1[i] - check_quat[i]) < epsilon);
        }
    }

    SUBCASE("Operator <<"){
        quaternion<float> quat1 = {-1, 3, 4, 3};
        std::stringstream out;
        out << quat1;

        CHECK(out.str() == "[-0.169031, 0.507093, 0.676123, 0.507093]");

    }

    SUBCASE("SetWithEuler()"){
        float epsilon = 0.00001;
        quaternion<float> return_quat;
        return_quat.SetWithEuler(0.6, -2.2, -3.68);
        float check_quat[4] = {0.138632, -0.85639, 0.0972236, -0.487776 };

        for(int i=0;i<4;i++){
            CHECK((return_quat[i] - check_quat[i])*(return_quat[i] - check_quat[i]) < epsilon);
        }
    }

    SUBCASE("Euler()"){

        SUBCASE(" < 90 && > -90 Degrees"){
            float epsilon = 0.000001;
            float check_values[4] = {0.138632, -0.85639, 0.0972236, -0.487776 };

            quaternion<float> values = {check_values[0], check_values[1], check_values[2], check_values[3] };
            float val[3];
            values.Euler(val);
            values.SetWithEuler(val[0], val[1], val[2]);

            for(int i=0;i<4;i++){
                CHECK((check_values[i] - values[i])*(check_values[i] - values[i]) < epsilon);
            }

        }

        SUBCASE("+ 90 Degrees"){
            float epsilon = 0.000001;
            float angles[3] = {1.57079632679, 1.57079632679, 30*3.14/180}; 
            quaternion<float> check;
            check.SetWithEuler(angles[0], angles[1],angles[2]);
            float *check_values = check.RawData();

            quaternion<float> values;
            values.SetWithEuler(angles[0], angles[1], angles[2]);
            float val[3];
            values.Euler(val);
            values.SetWithEuler(val[0], val[1], val[2]);

            for(int i=0;i<4;i++){
                CHECK((check_values[i] - values[i])*(check_values[i] - values[i]) < epsilon);
            }

        }

        SUBCASE("- 90 Degrees"){
            float epsilon = 0.00001;
            float angles[3] = {1.57079632679, -1.57079632679, 30*3.14/180}; 
            quaternion<float> check;
            check.SetWithEuler(angles[0], angles[1],angles[2]);
            float *check_values = check.RawData();

            quaternion<float> values;
            values.SetWithEuler(angles[0], angles[1], angles[2]);
            float val[3];
            values.Euler(val);
            values.SetWithEuler(val[0], val[1], val[2]);

            for(int i=0;i<4;i++){
                CHECK((check_values[i] - values[i])*(check_values[i] - values[i]) < epsilon);
            }

        }
        
    }

    SUBCASE("RotationMatrix3T()"){
        float epsilon = 0.000001;
        float check_matrix[9] = {
            -3.0/7.0, 18.0/35.0, 26.0/35.0,
            6.0/7.0, -1.0/35.0, 18.0/35.0,
            2.0/7.0, 6.0/7.0, -3.0/7.0
        };

        quaternion<float> quat1 = {-1, 3, 4, 3};

        float output[9];
        quat1.RotationMatrix3T(output);

        for(int i=0;i<9;i++){
            CHECK((output[i] - check_matrix[i])*(output[i] - check_matrix[i]) < epsilon);
        }
    }

    SUBCASE("RotationMatrix3()"){
        float epsilon = 0.000001;
        float check_matrix[9] = {
            -3.0/7.0, 6.0/7.0, 2.0/7.0,
            18.0/35.0, -1.0/35.0, 6.0/7.0,
            26.0/35.0, 18.0/35.0, -3.0/7.0
        };

        quaternion<float> quat1 = {-1, 3, 4, 3};

        float output[9];
        quat1.RotationMatrix3(output);

        for(int i=0;i<9;i++){
            CHECK((output[i] - check_matrix[i])*(output[i] - check_matrix[i]) < epsilon);
        }
    }

    SUBCASE("RotationMatrix4T()"){
        float epsilon = 0.000001;
        float check_matrix[16] = {
            -3.0/7.0, 18.0/35.0, 26.0/35.0, 0,
            6.0/7.0, -1.0/35.0, 18.0/35.0, 0,
            2.0/7.0, 6.0/7.0, -3.0/7.0, 0,
            0, 0, 0, 1
        };

        quaternion<float> quat1 = {-1, 3, 4, 3};

        float output[16];
        quat1.RotationMatrix4T(output);

        for(int i=0;i<16;i++){
            CHECK((output[i] - check_matrix[i])*(output[i] - check_matrix[i]) < epsilon);
        }
    }

    SUBCASE("RotationMatrix4()"){
        float epsilon = 0.000001;
        float check_matrix[16] = {
            -3.0/7.0, 6.0/7.0, 2.0/7.0, 0,
            18.0/35.0, -1.0/35.0, 6.0/7.0, 0,
            26.0/35.0, 18.0/35.0, -3.0/7.0, 0,
            0, 0, 0, 1
        };

        quaternion<float> quat1 = {-1, 3, 4, 3};

        float output[16];
        quat1.RotationMatrix4(output);

        for(int i=0;i<16;i++){
            CHECK((output[i] - check_matrix[i])*(output[i] - check_matrix[i]) < epsilon);
        }
    }

    SUBCASE("RawData()"){
        float check[4] = {-0.169030851, 0.507092553, 0.676123404, 0.507092553};

        quaternion<float> quat1 = {-1, 3, 4, 3};
        float *output = quat1.RawData();

        for(int i=0;i<4;i++){
            CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("Rotate()"){
        float check[3] = {-0.825714285, -5.59914285, 1.784571428};

        float output[3] = {-1.2, 0.37, -5.8}; // Point in Space

        quaternion<float> quat1 = {-1, 3, 4, 3};
        quat1.Rotate(output);

        for(int i=0;i<3;i++){
            CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("Conj()"){
        float check[4] = {-0.169030851, -0.507092553, -0.676123404, -0.507092553};

        quaternion<float> output = {-1, 3, 4, 3};
        output.Conj();

        for(int i=0;i<4;i++){
            CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
        }
    }

    SUBCASE("nlerp()"){
        SUBCASE("cos(theta) < 0"){
            float check[4] = {-0.167778, 0.633196, 0.649028, 0.386881};

            quaternion<float> q1 = {-1, 3, 4, 3};
            quaternion<float> q2 = {0.12, -3.159, -0.004, 2.15};

            quaternion<float> output;
            output.nlerp(q1, q2, .156);

            for(int i=0;i<4;i++){
                CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
            }
        }

        SUBCASE("Input t < 0"){
            float check[4] = {1, 0, 0, 0};
            bool is_error_thrown = false;

            quaternion<float> q1 = {-1, 3, 4, 3};
            quaternion<float> q2 = {0.12, -3.159, -0.004, 2.15};

            quaternion<float> output;
            try{
                output.nlerp(q1, q2, -.156);
            }
            catch(std::runtime_error e){
                is_error_thrown = true;
            }

            CHECK(is_error_thrown == true);

            for(int i=0;i<4;i++){
                CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
            }
        }

        SUBCASE("cos(theta) > 0"){
            float check[4] = {-0.167818, 0.633348, 0.648813, 0.386974};

            quaternion<float> q1 = {-1, 3, 4, 3};
            quaternion<float> q2 = {-0.12, 3.159, -0.004, -2.15};

            quaternion<float> output;
            output.nlerp(q1, q2, .156);

            for(int i=0;i<4;i++){
                CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
            }
        }

        SUBCASE("cos(theta) == 1"){
            float check[4] = {-0.169030851, 0.507092553, 0.676123404, 0.507092553};

            quaternion<float> q1 = {-1, 3, 4, 3};
            quaternion<float> q2 = {-1, 3, 4, 3};


            quaternion<float> output;
            output.nlerp(q1, q2, .156);

            for(int i=0;i<4;i++){
                CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
            }
        }

        SUBCASE("cos(theta) == -1"){
            float check[4] = {1, 0, 0, 0};
            bool is_error_thrown = false;

            quaternion<float> q1 = {-1, 3, 4, 3};
            quaternion<float> q2 = {1, -3, -4, -3};

            quaternion<float> output;
            try{
                output.nlerp(q1, q2, .156);
            }
            catch(std::runtime_error e){
                is_error_thrown = true;
            }

            CHECK(is_error_thrown == true);

            for(int i=0;i<4;i++){
                CHECK(output[i] == doctest::Approx( check[i] ).epsilon(0.000001));
            }
        }
    }

}

