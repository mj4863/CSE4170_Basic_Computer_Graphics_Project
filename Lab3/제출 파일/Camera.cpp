#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#include <vector>
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
    glm::mat3 R33_t;
    glm::mat4 T;

    switch (camera_id) {
    case CAMERA_MAIN:
        flag_valid = true;
        flag_move = true; // yes. the main camera is permitted to move

        // let's use glm funtions to set up the initial camera pose
        ViewMatrix = glm::lookAt(glm::vec3(-600.0f, -600.0f, 400.0f), glm::vec3(125.0f, 80.0f, 25.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        R33_t = glm::transpose(glm::mat3(ViewMatrix));
        T = glm::mat4(R33_t) * ViewMatrix;
        cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
        cam_proj.params.pers.aspect = win_aspect_ratio;
        cam_proj.params.pers.n = 1.0f;
        cam_proj.params.pers.f = 50000.0f;

        ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
            cam_proj.params.pers.n, cam_proj.params.pers.f);
        view_port.x = 200; view_port.y = 200; view_port.w = win_width - 200; view_port.h = win_height - 200;
        break;

    case CAMERA_CC_0:
        flag_valid = true;
        flag_move = false;  

        ViewMatrix = glm::lookAt(
            glm::vec3(70.0f, 150.0f, 40.0f),   
            glm::vec3(100.0f, 130.0f, 30.0f),  
            glm::vec3(0.0f, 0.0f, 1.0f)        
        );
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        R33_t = glm::transpose(glm::mat3(ViewMatrix));
        T = glm::mat4(R33_t) * ViewMatrix;
        cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 70.0f * TO_RADIAN;  
        cam_proj.params.pers.aspect = 1.0f;             
        cam_proj.params.pers.n = 1.0f;
        cam_proj.params.pers.f = 2000.0f;
        ProjectionMatrix = glm::perspective(
            cam_proj.params.pers.fovy,
            cam_proj.params.pers.aspect,
            cam_proj.params.pers.n,
            cam_proj.params.pers.f
        );

        view_port.x = win_width - 200;  
        view_port.y = win_height * 7 / 10; 
        view_port.w = 200;               
        view_port.h = 200;               
        break;

    case CAMERA_CC_1:
        flag_valid = true;
        flag_move = false;

        ViewMatrix = glm::lookAt(
            glm::vec3(20.0f, 70.0f, 40.0f),  
            glm::vec3(100.0f, 70.0f, 20.0f), 
            glm::vec3(0.0f, 0.0f, 1.0f)      
        );
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        R33_t = glm::transpose(glm::mat3(ViewMatrix));
        T = glm::mat4(R33_t) * ViewMatrix;
        cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 45.0f * TO_RADIAN;
        cam_proj.params.pers.aspect = 1.0f;
        cam_proj.params.pers.n = 1.0f;
        cam_proj.params.pers.f = 2000.0f;
        ProjectionMatrix = glm::perspective(
            cam_proj.params.pers.fovy,
            cam_proj.params.pers.aspect,
            cam_proj.params.pers.n,
            cam_proj.params.pers.f
        );

        view_port.x = win_width - 200;
        view_port.y = win_height * 4 / 10;  
        view_port.w = 200;
        view_port.h = 200;
        break;

    case CAMERA_CC_2:
        flag_valid = true;
        flag_move = false;

        ViewMatrix = glm::lookAt(
            glm::vec3(220.0f, 10.0f, 40.0f),    
            glm::vec3(160.0f, 120.0f, 25.0f),   
            glm::vec3(0.0f, 0.0f, 1.0f)         
        );
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        R33_t = glm::transpose(glm::mat3(ViewMatrix));
        T = glm::mat4(R33_t) * ViewMatrix;
        cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 60.0f * TO_RADIAN;
        cam_proj.params.pers.aspect = 1.0f;
        cam_proj.params.pers.n = 1.0f;
        cam_proj.params.pers.f = 2000.0f;
        ProjectionMatrix = glm::perspective(
            cam_proj.params.pers.fovy,
            cam_proj.params.pers.aspect,
            cam_proj.params.pers.n,
            cam_proj.params.pers.f
        );

        view_port.x = win_width - 200;
        view_port.y = win_height * 1 / 10;  
        view_port.w = 200;
        view_port.h = 200;
        break;

    case CAMERA_DYN_CCTV:
        flag_valid = true;
        flag_move = false;       

        ViewMatrix = glm::lookAt(
            glm::vec3(95.0f, 15.0f, 45.0f), 
            glm::vec3(55.0f, 40.0f, 0.0f),  
            glm::vec3(0.0f, 0.0f, 1.0f)    
        );
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        R33_t = glm::transpose(glm::mat3(ViewMatrix));
        T = glm::mat4(R33_t) * ViewMatrix;
        cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 45.0f * TO_RADIAN;  
        cam_proj.params.pers.aspect = win_aspect_ratio; 
        cam_proj.params.pers.n = 1.0f;               
        cam_proj.params.pers.f = 5000.0f;            
        ProjectionMatrix = glm::perspective(
            cam_proj.params.pers.fovy,
            cam_proj.params.pers.aspect,
            cam_proj.params.pers.n,
            cam_proj.params.pers.f
        );

        view_port.x = 0;
        view_port.y = 7 * (win_height / 10);
        view_port.w = win_width / 4;
        view_port.h = win_height / 4;

        is_dynamic = true;
        break;

    case CAMERA_TIGER_EYE:
        flag_valid = true;
        flag_move = false;  

        ViewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 20.0f),   
            glm::vec3(0.0f, 0.0f, 0.0f),    
            glm::vec3(0.0f, 1.0f, 0.0f)     
        );
        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        {
            glm::mat3 R33_t = glm::transpose(glm::mat3(ViewMatrix));
            glm::mat4 TT = glm::mat4(R33_t) * ViewMatrix;
            cam_view.pos = -glm::vec3(TT[3][0], TT[3][1], TT[3][2]);
        }
        cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
        cam_proj.params.pers.fovy = 45.0f * TO_RADIAN;
        cam_proj.params.pers.aspect = win_aspect_ratio;
        cam_proj.params.pers.n = 1.0f;
        cam_proj.params.pers.f = 500.0f;
        ProjectionMatrix = glm::perspective(
            cam_proj.params.pers.fovy,
            cam_proj.params.pers.aspect,
            cam_proj.params.pers.n,
            cam_proj.params.pers.f
        );
        
        view_port.x = 0;                    
        view_port.y = 0;
        view_port.w = win_width / 4;        
        view_port.h = win_height / 4;       
        break;

    default:
        break;
    }
}

void Orthographic_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
    switch (camera_id) {
    case CAMERA_SIDE_FRONT: {
        glm::vec3 eye = glm::vec3(125.0f, -300.0f, 25.0f);
        glm::vec3 center = glm::vec3(125.0f, 80.0f, 25.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        ViewMatrix = glm::lookAt(eye, center, up);

        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        {
            glm::mat3 R33_t = glm::transpose(glm::mat3(ViewMatrix));
            glm::mat4 T = glm::mat4(R33_t) * ViewMatrix;
            cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
        }
        flag_valid = true;
        flag_move = false;

        glm::vec3 worldMin(0.0f, 0.0f, 0.0f);
        glm::vec3 worldMax(240.0f, 160.0f, 100.0f);

        std::vector<glm::vec3> corners = {
            {worldMin.x, worldMin.y, worldMin.z},
            {worldMin.x, worldMin.y, worldMax.z},
            {worldMin.x, worldMax.y, worldMin.z},
            {worldMin.x, worldMax.y, worldMax.z},
            {worldMax.x, worldMin.y, worldMin.z},
            {worldMax.x, worldMin.y, worldMax.z},
            {worldMax.x, worldMax.y, worldMin.z},
            {worldMax.x, worldMax.y, worldMax.z}
        };

        float xmin = FLT_MAX, xmax = -FLT_MAX;
        float ymin = FLT_MAX, ymax = -FLT_MAX;
        for (auto& p : corners) {
            glm::vec4 cp = ViewMatrix * glm::vec4(p, 1.0f);
            xmin = std::min(xmin, cp.x);
            xmax = std::max(xmax, cp.x);
            ymin = std::min(ymin, cp.y);
            ymax = std::max(ymax, cp.y);
        }

        cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
        cam_proj.params.ortho.left = xmin;
        cam_proj.params.ortho.right = xmax;
        cam_proj.params.ortho.bottom = ymin;
        cam_proj.params.ortho.top = ymax;
        cam_proj.params.ortho.n = 1.0f;   
        cam_proj.params.ortho.f = 1000.0f;

        ProjectionMatrix = glm::ortho(
            xmin, xmax,
            ymin, ymax,
            cam_proj.params.ortho.n,
            cam_proj.params.ortho.f
        );

        view_port.x = 0;
        view_port.y = win_height * 7 / 10;
        view_port.w = win_width / 4;
        view_port.h = win_height / 4;

        break;
    }

    case CAMERA_TOP: {
        glm::vec3 eye = glm::vec3(125.0f, 80.0f, 500.0f);
        glm::vec3 center = glm::vec3(125.0f, 80.0f, 25.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        ViewMatrix = glm::lookAt(eye, center, up);

        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        {
            glm::mat3 R33_t = glm::transpose(glm::mat3(ViewMatrix));
            glm::mat4 T = glm::mat4(R33_t) * ViewMatrix;
            cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
        }
        flag_valid = true;
        flag_move = false;

        glm::vec3 worldMin(0.0f, 0.0f, 0.0f);
        glm::vec3 worldMax(240.0f, 200.0f, 200.0f);

        std::vector<glm::vec3> corners = {
            {worldMin.x, worldMin.y, worldMin.z},
            {worldMin.x, worldMin.y, worldMax.z},
            {worldMin.x, worldMax.y, worldMin.z},
            {worldMin.x, worldMax.y, worldMax.z},
            {worldMax.x, worldMin.y, worldMin.z},
            {worldMax.x, worldMin.y, worldMax.z},
            {worldMax.x, worldMax.y, worldMin.z},
            {worldMax.x, worldMax.y, worldMax.z}
        };

        float xmin = FLT_MAX, xmax = -FLT_MAX;
        float ymin = FLT_MAX, ymax = -FLT_MAX;
        for (auto& p : corners) {
            glm::vec4 cp = ViewMatrix * glm::vec4(p, 1.0f);
            xmin = std::min(xmin, cp.x);
            xmax = std::max(xmax, cp.x);
            ymin = std::min(ymin, cp.y);
            ymax = std::max(ymax, cp.y);
        }

        cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
        cam_proj.params.ortho.left = xmin;
        cam_proj.params.ortho.right = xmax;
        cam_proj.params.ortho.bottom = ymin;
        cam_proj.params.ortho.top = ymax;
        cam_proj.params.ortho.n = 1.0f;
        cam_proj.params.ortho.f = 1000.0f;

        ProjectionMatrix = glm::ortho(
            xmin, xmax,
            ymin, ymax,
            cam_proj.params.ortho.n,
            cam_proj.params.ortho.f
        );

        view_port.x = 0;
        view_port.y = win_height * 4 / 10;
        view_port.w = win_width / 4;
        view_port.h = win_height / 4;

        break;
    }

    case CAMERA_SIDE: {
        glm::vec3 eye = glm::vec3(300.0f, 60.0f, 25.0f);
        glm::vec3 center = glm::vec3(125.0f, 60.0f, 25.0f);
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f); 
        ViewMatrix = glm::lookAt(eye, center, up);

        cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
        {
            glm::mat3 R33_t = glm::transpose(glm::mat3(ViewMatrix));
            glm::mat4 T = glm::mat4(R33_t) * ViewMatrix;
            cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
        }
        flag_valid = true;
        flag_move = false;

        glm::vec3 worldMin(0.0f, 0.0f, 0.0f);
        glm::vec3 worldMax(240.0f, 200.0f, 100.0f);

        std::vector<glm::vec3> corners = {
            {worldMin.x, worldMin.y, worldMin.z},
            {worldMin.x, worldMin.y, worldMax.z},
            {worldMin.x, worldMax.y, worldMin.z},
            {worldMin.x, worldMax.y, worldMax.z},
            {worldMax.x, worldMin.y, worldMin.z},
            {worldMax.x, worldMin.y, worldMax.z},
            {worldMax.x, worldMax.y, worldMin.z},
            {worldMax.x, worldMax.y, worldMax.z}
        };

        float xmin = FLT_MAX, xmax = -FLT_MAX;
        float ymin = FLT_MAX, ymax = -FLT_MAX;
        for (auto& p : corners) {
            glm::vec4 cp = ViewMatrix * glm::vec4(p, 1.0f);
            xmin = std::min(xmin, cp.x);
            xmax = std::max(xmax, cp.x);
            ymin = std::min(ymin, cp.y);
            ymax = std::max(ymax, cp.y);
        }

        cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
        cam_proj.params.ortho.left = xmin;
        cam_proj.params.ortho.right = xmax;
        cam_proj.params.ortho.bottom = ymin;
        cam_proj.params.ortho.top = ymax;
        cam_proj.params.ortho.n = 1.0f;
        cam_proj.params.ortho.f = 1000.0f;

        ProjectionMatrix = glm::ortho(
            xmin, xmax,
            ymin, ymax,
            cam_proj.params.ortho.n,
            cam_proj.params.ortho.f
        );

        view_port.x = 0;
        view_port.y = win_height * 1 / 10;
        view_port.w = win_width / 4;
        view_port.h = win_height / 4;

        break;
    }

    default:
        break;
    }
}
