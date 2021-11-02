<div id="header">

   [![Issues][issues-shield]][issues-url]
   [![Apache 2.0 License][license-shield]][license-url]

   [issues-shield]: https://img.shields.io/github/issues/mattie20/Arcball-Graphics-Package
   [issues-url]: https://github.com/mattie20/Arcball-Graphics-Package/issues
   [license-shield]: https://img.shields.io/github/license/mattie20/Arcball-Graphics-Package.svg?style=for-the-badge
   [license-url]: https://github.com/mattie20/Arcball-Graphics-Package/blob/master/LICENSE.txt

   <h1 align="center">Simple Arcball Package</h1>
</div>


  <p align="center">
    <a href="https://github.com/mattie20/Arcball-Graphics-Package/issues"><strong>Report Bug</strong></a>
    ·
    <a href="https://github.com/mattie20/Arcball-Graphics-Package/issues"><strong>Request Feature</strong></a>
  </p>
    <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#arcball">Arcball</a></li>
        <li><a href="#quaternion">Quaternion</a></li>
        <li><a href="#other-functions">Other Functions</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>



## About The Project
SAP is a simple header only library containing a templated quaternion implementation, a arcball implementation ( improved over traditional arcball, see white paper in repo), and a few other functions required to create MVP matricies. The library is written to be auto-vectorized when possible with the appropriate auto-vectorization flags per compiler. Unit testing implemented with doctest.



## Getting Started
This is an example of common usage of the library. I've tried to make the functionality
as self-evident and obvious as possible.



## `Arcball`

1. Initalizaiton
   ```c++
   arcball arc;

   arc.SetCamera(camera, up_vector);
   arc.SetCenter(center_pos);
   arc.SetViewArea(window_width,window_height);

   // can omit these if okay with default values
   arc.SetProjectionVars(fov, z_near, z_far); 
   arc.rotate_sensitivity = 0.004;
   arc.zoom_sensitivity = 0.2;
   arc.zoom_translate_sensitivity = 0.1;
   ```
2. Rotate Function
   ```c++
   if(is_scroll_clicked){
      arc.Rotate(mouse_delta_x, mouse_delta_y);
      camera_pos = arc.Camera(); // If Needed for Shader
      click_delta_x = 0;
      click_delta_y = 0;
   }
   ```
4. Zoom Function
   ```c++
   if(is_scrolled){
      // Distance From Center of Screen. Left and Up are Positive
      // Float Type Because of Possible Subpixel Resolution From OS
      float dis_x = 0.5*window_width - mouse_x;
      float dis_y = 0.5*window_height - mouse_y;

      arc.Zoom(dis_x, dis_y, scroll_ammount);
      camera_pos = arc.Camera(); // If Needed for Shader
      is_scrolled = false;
   }
   ```
5. Translate Function
   ```c++
   if(is_left_clicked){
      arc.Translate(mouse_delta_x, mouse_delta_y);
      camera_pos = arc.Camera(); // If Needed for Shader
   }
   ```
5. ViewProjMatrix Function
   ```c++
   // Creates View Projection Matrix in Location Pointed to by viewproj
   float viewproj[16];
   arc.ViewProjMatrix(viewproj);
   ```
6. MouseRay Function
   ```c++
   // Gets Direction Vector of Mouse Ray From Camera
   float ray[3];
   arc.MouseRay(ray);
   ```

<p align="right">(<a href="#top">back to top</a>)</p>



## `Quaternion`

Initalizaiton
   ```c++
   quaternion<float> quat1 = {-1, 3, 4, 3};
   // Or
   quaternion<float> quat3;
   ```
Multiplication
   ```c++
   // Returns New Quat
   quat3 = quat1 * quat2;
   ```
Element Access
   ```c++
   // Gets Specific Element. Stored Order: w, x, y, z
   quaternion<float> quat1;
   float w = quat1[0];
   ```
Set With Euler Angles
   ```c++
   // Sets Quaternion with Euler Conversion using ZYX Matrix Order
   // Inputs in Radians. Order: x-axis Angle, y-axis Angle, z-axis Angle
   quat1.SetWithEuler(0.2,-1.1,2.1);
   ```
Get Euler Angles
   ```c++
   // Gets Quaternion Euler Conversion using ZYX Matrix Order
   // Outputs in Radians. Order: x-axis Angle, y-axis Angle, z-axis Angle
   // Note: Euler Values Returned May Not Be Unique
   float val[3];
   quat1.Euler(val);
   ```
Get Raw Data
   ```c++
   // Gets Pointer to Data
   float *data = quat1.RawData();
   ```
Conjugate Quaternion
   ```c++
   // Conjugates Quaternion
   quat1.Conj()
   ```

Get Rotation Matrix
   ```c++
   // Sets Rotation Matrix to matrix
   // T Ending Denotes Transposed Matrix
   float matrix[9];
   quat1.RotationMatrix3(matrix);
   quat1.RotationMatrix3T(matrix);
   // Or
   float matrix[16];
   quat1.RotationMatrix4(matrix);
   quat1.RotationMatrix4T(matrix);
   ```
Rotate Point
   ```c++
   // Rotates Point with Quaternion
   quaternion<float> quat1 = {-1, 3, 4, 3};
   float point = {1, 2, 5.5};
   quat1.Rotate(point);
   ```
nlerp
```c++
   // Creates rotation quaternion t percentage from quat1 to quat2
   quaternion<float> quat1 = {-1, 3, 4, 3};
   quaternion<float> quat2 = {-1, 2, 1, 1};
   quaternion<float> quat3;
   float t = .15; // Percentage Interpolation

   quat3.nlerp(quat1, quat2, t)

   ```
Ostream Operator
```c++
   // Creates rotation quaternion t percentage from quat1 to quat2
   quaternion<float> quat1 = {-1, 3, 4, 3};
   std::cout<<quat1<<std::endl; // Prints [w, x, y, z]

   ```

## `Other Functions`

   Multiply a Matrix with Another Matrix Transposed
   ```c++
   // Multiplies a Row Major Matrix with a Column Major (Row Major Transposed) Matrix 
   // to Produce a Row Major Matrix

   float mat4_1[16];
   float mat4_2T[16];
   float mat4_out[16];
   Mat4MultiplyMat4T(mat4_1, mat4_2T, mat4_out);
   ```
Cross Product of two 3 Dimensional Vectors
   ```c++
   // Computes Cross Product
   float vec_1[3] = {0.8, 3.9, 2.1};
   float vec_2[3] = {1.5, 3.3, 1.2};
   float return_vec[3];
   CrossVec3(vec_1, vec_2, return_vec);
   ```
Normalize a 3 Dimensional Vector
   ```c++
   // Computes Cross Product
   float vec_1[3] = {0.8, 3.9, 2.1};
   float vec_2[3] = {1.5, 3.3, 1.2};
   float return_vec[3];
   CrossVec3(vec_1, vec_2, return_vec);
   ```
Prints a 4x4 matrix
   ```c++
   // Computes Cross Product
   float matrix[16];
   PrintMat4(matrix, "MatrixName");
   ```
Calculate Magnitude of a Vec3
   ```c++
   MagnitudeVec3(vec3);
   ```
Calculate Difference of a Vec3
   ```c++
   // Vec3_out = Vec3_1 - Vec3_2
   float vec3_1[3];
   float vec3_2[3];
   float vec3_out[3];
   DiffVec3(vec3_1, vec3_2, vec3_out);
   ```


<!-- _For more examples, please refer to the [Documentation](https://example.com)_ -->

<p align="right">(<a href="#header">back to top</a>)</p>



## Contributing

If you have a suggestion that would make this project better, simply open an issue with the tag "enhancement". 
Don't forget to give the project a star! Thanks again!

See the [open issues](https://github.com/mattie20/Arcball-Graphics-Package/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#header">back to top</a>)</p>



## License

Distributed under the Apache 2.0 License. See `LICENSE.txt` for more information.



## Contact

Matthew Elks - mattelks43216@gmail.com

Github: [https://github.com/github_username](https://github.com/mattie20)



## Acknowledgments

* [Best-README-Template](https://github.com/othneildrew/Best-README-Template)

<p align="right">(<a href="#header">back to top</a>)</p>