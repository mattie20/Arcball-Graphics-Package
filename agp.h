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


//  "The engines don’t move the ship at all. The ship stays where it is 
//  and the engines move the universe around it" -Futurama


#ifndef SIMPLE_ARCBALL_PACKAGE_H_
#define SIMPLE_ARCBALL_PACKAGE_H_


#include<cmath>
#include<stdexcept>
#include<iostream>
#include<initializer_list>


// result = matrix_1 * matrix_2 in row major
void Mat4MultiplyMat4T(const float * __restrict__ mat4_1, 
const float * __restrict__ mat4_2, float * __restrict__ out){
    for (int i=0; i<4; i++){
        for (int j=0; j<4; j++){
               out[i*4 + j] = 0;
            for (int k=0; k<4; k++){
                out[i*4 + j] += mat4_1[i*4 + k]*mat4_2[j*4 + k];
            }
        }
    }
}


void PrintMat4(const float *matrix, const char* name){
    std::cout<<name<<" = { ";
    int spaces = sizeof(name)/sizeof(name[0]);
    int dim = 4; // Can Make for Arbitrary Square Dim
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim - 1;j++){
            std::cout<<std::to_string(matrix[i*dim + j])<<", ";
        }
        std::cout<<std::to_string(matrix[i*dim + dim - 1]);
        if(i < dim - 1){
            std::cout<<", \n";
            for(int j=0;j<spaces;j++){
                std::cout<<" ";
            }
        }
    }
    std::cout<<" }\n";
}


void NormalizeVec3(float *vec3){
    float magnitude = 0;
    for(int i=0; i<3; i++){
        magnitude += vec3[i]*vec3[i];
    }
    magnitude = 1/sqrt(magnitude);

    for(int i=0; i<3; i++){
        vec3[i] = vec3[i]*magnitude;
    }
}


void CrossVec3(const float * __restrict__ left_vec, 
const float * __restrict__ right_vec, float * __restrict__ return_vec){
    return_vec[0] = left_vec[1]*right_vec[2] - left_vec[2]*right_vec[1];
    return_vec[1] = left_vec[2]*right_vec[0] - left_vec[0]*right_vec[2];
    return_vec[2] = left_vec[0]*right_vec[1] - left_vec[1]*right_vec[0];
}


float DotVec3(const float * __restrict__ vec1, const float * __restrict__ vec2){
    float output = 0;
    for (int i=0; i<3; i++){
        output += vec1[i]*vec2[i];
    }
    return output;
}


// outvec = vec1 - vec2
void DiffVec3(const float * __restrict__ vec1, const float * __restrict__ vec2,
 float * __restrict__ outvec){
    for (int i=0; i<3; i++){
        outvec[i] = vec1[i] - vec2[i];
    }
}


float MagnitudeVec3(const float *vec){
    float output = 0;
    for (int i=0; i<3; i++){
        output += vec[i]*vec[i];
    }
    return sqrt(output);
}


//  "The engines don’t move the ship at all. The ship stays where it is 
//  and the engines move the universe around it" -Futurama
struct arcball{

float rotate_sensitivity = 0.004;

float zoom_sensitivity = 0.2;

float zoom_translate_sensitivity = 0.1;

void SetCamera(const float *cam_pos, const float *up){

    // Find Cos(theta) between the two vectors
    float cam_mag = 0;
    float up_mag = 0;
    for(int i=0; i<3; i++){
        cam_mag += cam_pos[i]*cam_pos[i];
        up_mag += up[i]*up[i];
    }
    float dotprod = DotVec3(cam_pos, up)/(sqrt(cam_mag)*sqrt(up_mag));
    float dir_vec[3];

    if( dotprod >= 0.999){
        // Vectors are Facing the Same Direction
        throw std::runtime_error("Camera and Up Vectors Parallel");
    }
    else if( dotprod >= 0.00001){
        // If vectors aren't perpendicular, make them perpendicular
        DiffVec3(cam_pos, center_pos, dir_vec);

        float right[3];
        CrossVec3(up, dir_vec, right);
        CrossVec3(dir_vec, right, up_vec);
    }
    else{
        // Vectors are Perpendicular
        std::copy(up, up + 3, up_vec);
        DiffVec3(cam_pos, center_pos, dir_vec);
    }

    std::copy(cam_pos, cam_pos + 3, camera_pos);
    NormalizeVec3(up_vec);
    FormBasis();

    radius = MagnitudeVec3(dir_vec);
}

void SetCenter(const float *input){
    std::copy(input, input+3, center_pos);
    FormBasis();
    float dir_vec[3];
    DiffVec3(camera_pos, center_pos, dir_vec);
    radius = radius = MagnitudeVec3(dir_vec);
}

void SetRadius(const float input){
    if(input < 0){throw std::runtime_error("Radius Negative");}
    float dir_vec[3];
    DiffVec3(camera_pos, center_pos, dir_vec);
    NormalizeVec3(dir_vec);
    radius = input;
    for (int i=0; i<3; i++){
        camera_pos[i] = radius*dir_vec[i] + center_pos[i];
    }
    FormBasis();
}

const float *Camera(){return camera_pos;}

const float *Center(){return center_pos;}

void SetProjectionVars(const float fov, const float z_near, const float z_far){
    // Change Projection Matrix Values
    float tangent = tan(0.5*fov);
    pixel_to_wspace_x = m00*pixel_to_wspace_x;
    pixel_to_wspace_y = m11*pixel_to_wspace_y;
    m00 = 1/(aspect_ratio*tangent);
    m11 = 1/tangent;
    pixel_to_wspace_x = aspect_ratio*tangent*pixel_to_wspace_x;
    pixel_to_wspace_y = tangent*pixel_to_wspace_y;
    m22 = (z_near + z_far)/(z_near - z_far);
    m32 = 2*z_near*z_far/(z_near - z_far);
}


// out_vec is a 1x3 vector containing the direction vector of the ray
void MouseRay(const float mouse_x, const float mouse_y, float *out_vec){

    // Find Local Direction Vector
    float vec[3] = {-mouse_x*pixel_to_wspace_x, mouse_y*pixel_to_wspace_y, -1};

    // Multiply Vector by the Basis Matrix Transposed
    for (int i=0; i<3; i++){
        out_vec[i] = basis[i]*vec[0] + basis[i + 3]*vec[1] + basis[i + 6]*vec[2];
    }
}


void SetViewArea(const int window_width, const int window_height){
    aspect_ratio = (float)window_width/(float)window_height;
    m00 = m11*(1/aspect_ratio); // Projection Matrix Value Changes with Aspect Ratio
    pixel_to_wspace_x = 1/(m00*0.5*window_width);
    pixel_to_wspace_y = 1/(m11*0.5*window_height);
}


void Translate(const float delta_x, const float delta_y){

    // Transform Screen Vector into Gobal Coordinates and Add to Camera and Center Position
    if(delta_x == 0 && delta_y == 0){return;}
    else{
        // Create Local Vector
        float vec[2] = {-delta_x*radius*pixel_to_wspace_x, delta_y*radius*pixel_to_wspace_y};

        // Multiply Vector by the Basis Matrix Transposed
        for (int i = 0; i < 3; i++){
            float temp = basis[i]*vec[0] + basis[i + 3]*vec[1];
            camera_pos[i] += temp;
            center_pos[i] += temp;
        }
    }
}


void Zoom(const float mouse_x, const float mouse_y, const float zoom){

    // translate center and camera_pos to new mouse coordinates
    if(zoom < 0){
        float vec[2] = {-zoom_translate_sensitivity*mouse_x*radius*pixel_to_wspace_x, zoom_translate_sensitivity*mouse_y*radius*pixel_to_wspace_y};

        // Multiply Vector by the Basis Matrix Transposed
        for (int i = 0; i < 3; i++){
            float temp = basis[i]*vec[0] + basis[i + 3]*vec[1];
            camera_pos[i] += temp;
            center_pos[i] += temp;
        }
    }


    radius += zoom_sensitivity*zoom;

    if(radius < 0){
        radius = .001;
    }

    float dir_vec[3];
    DiffVec3(camera_pos, center_pos, dir_vec);
    NormalizeVec3(dir_vec);
    for (int i=0; i<3; i++){
        camera_pos[i] = dir_vec[i]*radius + center_pos[i];
    }


}


void Rotate(const float delta_x, const float delta_y){

    if(delta_x == 0 && delta_y == 0){return;}
    else{
        // Calculate local position vector
        float magnitude = sqrt(delta_x*delta_x + delta_y*delta_y);
        float theta = rotate_sensitivity*magnitude;
        float sine = sin(theta)/magnitude;
        float cosine = cos(theta);
        float multiplier = -delta_y*(1 - cosine)/(magnitude*magnitude);

        float vec[3] = { -delta_x*radius*sine, delta_y*radius*sine, radius*cosine - radius};

        float vec2[3] = {delta_x*multiplier, delta_y*multiplier, - delta_y*sine};

        // Multiply Transposed Basis Matrix by the Unit Vector and Add
        for(int i=0; i<3; i++){
            // Dumb Down the Code for Auto-Vectorization
            for (int j=0; j<3; j++){
                camera_pos[i] += basis[i + j*3]*vec[j];
                up_vec[i] += basis[i + j*3]*vec2[j];
            }
        }
    }
}


void ViewProjMatrix(float *matrix){
    // Create Basis to Local Space from Global Space... "View Matrix"
    FormBasis();

    // Matrix Multiplication of View Matrix With the Sparse Projection Matrix
    matrix[3] = -DotVec3(camera_pos, basis)*m00;
    matrix[7] = -DotVec3(camera_pos, basis + 3)*m11;
    matrix[15] = DotVec3(camera_pos, basis + 6);
    matrix[11] = -matrix[15]*m22 + m32;

    for(int i=0; i<3; i++){
        matrix[i] = basis[i]*m00;
        matrix[i + 4] = basis[i + 3]*m11;
        matrix[i + 8] = basis[i + 6]*m22;
        matrix[i + 12] = -basis[i + 6];
    }
}


private:

void FormBasis(){
    for (int i=0; i<3; i++){
        basis[i + 6] = camera_pos[i] - center_pos[i];
    }
    NormalizeVec3(basis + 6);
    std::copy(up_vec, up_vec + 3, basis + 3);
    CrossVec3(basis + 3, basis + 6, basis);
    NormalizeVec3(basis);
}

float basis[9];

float center_pos[3] = {0,0,0};

float camera_pos[3] = {0,1,0};

float up_vec[3] = {0,0,1};

float radius = 1;

float aspect_ratio = 1;

float pixel_to_wspace_x;

float pixel_to_wspace_y;

// Projection Matrix Values
float m00 = 1/(aspect_ratio*tan(0.5*(40*3.14/180)));

float m11 = 1/tan(0.5*(40*3.14/180));

float m22 = (0.1 + 100)/(0.1 - 100);

float m32 = 2*0.1*100/(0.1 - 100);

};



template <typename T> class quaternion{

T quat[4] = {1,0,0,0};

public:


// Constructors
quaternion(){};

quaternion(std::initializer_list<T> init){
    std::copy(init.begin(), init.end(), quat);
    Normalize();
};


// Destructor
~quaternion(){};


// Copy Constructor
quaternion(const quaternion &q2){
    std::copy(q2.quat, q2.quat + 4, quat);
}


// Operators
T& operator[] (int pos){
    if(pos > 0 || pos < 4){
        return this->quat[pos];
    }
    else{
        throw std::runtime_error("Index Out of Bounds");
    }
}

void operator= (const quaternion &q2){
    std::copy(q2.quat, q2.quat + 4, quat);
}
    

// Ostream Print
friend std::ostream& operator<< (std::ostream& os, const quaternion& qt){
    os<<"["<<qt.quat[0]<<", "<<qt.quat[1]<<", "<<qt.quat[2]<<", "<<qt.quat[3]<<"]";
    return os;
}


// Member Functions

// q_return = q1 * q2
quaternion operator* (const quaternion &q2){
    quaternion<T> return_quat;
    return_quat[0] = quat[0]*q2.quat[0] - quat[1]*q2.quat[1] - quat[2]*q2.quat[2] - quat[3]*q2.quat[3];
    return_quat[1] = quat[0]*q2.quat[1] + quat[1]*q2.quat[0] + quat[2]*q2.quat[3] - quat[3]*q2.quat[2];
    return_quat[2] = quat[0]*q2.quat[2] - quat[1]*q2.quat[3] + quat[2]*q2.quat[0] + quat[3]*q2.quat[1];
    return_quat[3] = quat[0]*q2.quat[3] + quat[1]*q2.quat[2] - quat[2]*q2.quat[1] + quat[3]*q2.quat[0];

    return_quat.Normalize();
    return return_quat;
}

// Returns Pointer To Quat Array
T* RawData(){
    return this->quat;
}

quaternion & Conj(){
    for(int i=1; i<4; i++){
        quat[i] = -quat[i];
    }
    return *this;
}

void Rotate(float *vec){
    float q[3] = {quat[1], quat[2], quat[3]};
    float qcrossr[3];
    float qright[3];

    CrossVec3(q, vec, qcrossr);

    for(int i=0; i<3; i++){
        q[i] = 2*q[i];
    }
    CrossVec3(q, qcrossr, qright);

    for(int i=0; i<3; i++){
        vec[i] += 2*quat[0]*qcrossr[i] + qright[i];
    }

}


// Returns 4x4 Rotation Matrix
void RotationMatrix4(T *matrix){
    
    matrix[0] = 2*(quat[0]*quat[0] + quat[1]*quat[1]) - 1; // 0,0
    matrix[1] = 2*(quat[1]*quat[2] - quat[0]*quat[3]);     // 0,1
    matrix[2] = 2*(quat[1]*quat[3] + quat[0]*quat[2]);     // 0,2
    matrix[3] = 0;
    matrix[4] = 2*(quat[1]*quat[2] + quat[0]*quat[3]);     // 1,0
    matrix[5] = 2*(quat[0]*quat[0] + quat[2]*quat[2]) - 1; // 1,1
    matrix[6] = 2*(quat[2]*quat[3] - quat[0]*quat[1]);     // 1,2
    matrix[7] = 0;
    matrix[8] = 2*(quat[1]*quat[3] - quat[0]*quat[2]);     // 2,0
    matrix[9] = 2*(quat[2]*quat[3] + quat[0]*quat[1]);     // 2,1
    matrix[10] = 2*(quat[0]*quat[0] + quat[3]*quat[3]) - 1; // 2,2
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1; // 3, 3
}

// Returns 4x4 Rotation Matrix Transposed
void RotationMatrix4T(T *matrix){

    matrix[0] = 2*(quat[0]*quat[0] + quat[1]*quat[1]) - 1; // 0,0
    matrix[1] = 2*(quat[1]*quat[2] + quat[0]*quat[3]);     // 1,0
    matrix[2] = 2*(quat[1]*quat[3] - quat[0]*quat[2]);     // 2,0
    matrix[3] = 0;
    matrix[4] = 2*(quat[1]*quat[2] - quat[0]*quat[3]);     // 0,1
    matrix[5] = 2*(quat[0]*quat[0] + quat[2]*quat[2]) - 1; // 1,1
    matrix[6] = 2*(quat[2]*quat[3] + quat[0]*quat[1]);     // 2,1
    matrix[7] = 0;
    matrix[8] = 2*(quat[1]*quat[3] + quat[0]*quat[2]);     // 0,2
    matrix[9] = 2*(quat[2]*quat[3] - quat[0]*quat[1]);     // 1,2
    matrix[10] = 2*(quat[0]*quat[0] + quat[3]*quat[3]) - 1; // 2,2
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1; // 3, 3
}

// Returns 3x3 Rotation Matrix
void RotationMatrix3(T *matrix){


    matrix[0] = 2*(quat[0]*quat[0] + quat[1]*quat[1]) - 1; // 0,0
    matrix[1] = 2*(quat[1]*quat[2] - quat[0]*quat[3]);     // 0,1
    matrix[2] = 2*(quat[1]*quat[3] + quat[0]*quat[2]);     // 0,2
    matrix[3] = 2*(quat[1]*quat[2] + quat[0]*quat[3]);     // 1,0
    matrix[4] = 2*(quat[0]*quat[0] + quat[2]*quat[2]) - 1; // 1,1
    matrix[5] = 2*(quat[2]*quat[3] - quat[0]*quat[1]);     // 1,2
    matrix[6] = 2*(quat[1]*quat[3] - quat[0]*quat[2]);     // 2,0
    matrix[7] = 2*(quat[2]*quat[3] + quat[0]*quat[1]);     // 2,1
    matrix[8] = 2*(quat[0]*quat[0] + quat[3]*quat[3]) - 1; // 2,2
}

// Returns 3x3 Rotation Matrix Transposed
void RotationMatrix3T(T *matrix){

    matrix[0] = 2*(quat[0]*quat[0] + quat[1]*quat[1]) - 1; // 0,0
    matrix[1] = 2*(quat[1]*quat[2] + quat[0]*quat[3]);     // 1,0
    matrix[2] = 2*(quat[1]*quat[3] - quat[0]*quat[2]);     // 2,0
    matrix[3] = 2*(quat[1]*quat[2] - quat[0]*quat[3]);     // 0,1
    matrix[4] = 2*(quat[0]*quat[0] + quat[2]*quat[2]) - 1; // 1,1
    matrix[5] = 2*(quat[2]*quat[3] + quat[0]*quat[1]);     // 2,1
    matrix[6] = 2*(quat[1]*quat[3] + quat[0]*quat[2]);     // 0,2
    matrix[7] = 2*(quat[2]*quat[3] - quat[0]*quat[1]);     // 1,2
    matrix[8] = 2*(quat[0]*quat[0] + quat[3]*quat[3]) - 1; // 2,2
}


// Sets Quaternion With Euler Angles
// Angles must be in radians
// NASA ZYX Rotation Order
void SetWithEuler(float roll/*x*/, float pitch/*y*/, float yaw/*z*/){
    T cos_z = cos(0.5*yaw);
    T sin_z = sin(0.5*yaw);
    T cos_y = cos(0.5*pitch);
    T sin_y = sin(0.5*pitch);
    T cos_x = cos(0.5*roll);
    T sin_x = sin(0.5*roll);

    T cxcy = cos_x * cos_y;
    T sxsy = sin_x * sin_y;
    T sxcy = sin_x * cos_y;
    T cxsy = cos_x * sin_y;

    quat[0] = cxcy * cos_z + sxsy * sin_z;
    quat[1] = sxcy * cos_z - cxsy * sin_z;
    quat[2] = cxsy * cos_z + sxcy * sin_z;
    quat[3] = cxcy * sin_z - sxsy * cos_z;

    // Normalization
    Normalize();
}

// Angles must be in radians
// NASA ZYX Rotation Order
void Euler(float *output){

	T cross = quat[0]*quat[2] - quat[3]*quat[1];

    // Aligned With Positive Z-axis
	if(cross > 0.49999){
        output[0] = 2*atan2(quat[1], quat[0]);
		output[1] = 1.57079632679;
		output[2] = 0;
		return;
	}

    // Aligned With Negative Z-axis
	else if(cross < -0.49999){
        output[0] = 2*atan2(quat[1], quat[0]);
		output[1] = -1.57079632679;
		output[2] = 0;
		return;
	}
    
    else{
        output[0] = atan2(2*(quat[0]*quat[1] + quat[2]*quat[3]) , 1 - 2*(quat[1]*quat[1] + quat[2]*quat[2]));
        output[1] = asin(2*cross);
        output[2] = atan2(2*(quat[0]*quat[3] + quat[1]*quat[2]) , 1 - 2*(quat[2]*quat[2] + quat[3]*quat[3]));
        return;
    }
}


// returns nlerp Quaternion From q1 To q2 by Percentage t Between 0 and 1
void nlerp(quaternion &q1, quaternion &q2, float t){

        if(t < 0 || t > 1){throw std::runtime_error("Out of Bounds Percentage");};

        float angle = 0;
        for(int i=0; i<4; i++){
            angle += q1[i]*q2[i];
        }

        if(angle < 0.0){
            if(angle < -0.999){throw std::runtime_error("nlerp Undefined at 180 Degrees");}
            for(int i=0; i<4; i++){
                quat[i] = q1[i] - t*(q1[i] + q2[i]);
            }
        }
        else{
            for(int i=0; i<4; i++){
                quat[i] = q1[i] - t*(q1[i] - q2[i]);
            }
        }
        
        Normalize();
}


private: 

// Normalize Internal Quat Array
void Normalize(){
    T magnitude = 0;
    for (int i=0; i<4; i++){
        magnitude += quat[i]*quat[i];
    }
    magnitude = 1/sqrt(magnitude);
    for (int i=0; i<4; i++){
        quat[i] = quat[i]*magnitude;
    }
}

};


#endif