#pragma once

#define TO_RADIAN 0.01745329252f  

#include <glm/gtc/matrix_transform.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC
};

enum Camera_ID {
	CAMERA_MAIN = 0,
	CAMERA_SIDE_FRONT,
	CAMERA_TOP,
	CAMERA_SIDE,
	CAMERA_CC_0,      
	CAMERA_CC_1,      
	CAMERA_CC_2,      
	CAMERA_DYN_CCTV,  
	CAMERA_TIGER_EYE
};

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
};

struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct {
			float left, right, bottom, top, n, f;
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;

	Camera(Camera_ID _camera_id) : camera_id(_camera_id) {}
	bool flag_valid;
	bool flag_move; // what else?
};

struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);

	float zoomSpeed = 1.0f * TO_RADIAN;		
	bool  is_dynamic = false;				
};

struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
};

struct Camera_Data {
	Perspective_Camera cam_main{ CAMERA_MAIN };
	Orthographic_Camera cam_front{ CAMERA_SIDE_FRONT };		
	Orthographic_Camera cam_top{ CAMERA_TOP };				
	Orthographic_Camera cam_side{ CAMERA_SIDE };			
	Perspective_Camera cctv_0{ CAMERA_CC_0 };				
	Perspective_Camera cctv_1{ CAMERA_CC_1 };
	Perspective_Camera cctv_2{ CAMERA_CC_2 };
	Perspective_Camera dynamic_cctv{ CAMERA_DYN_CCTV };		
	Perspective_Camera cam_tiger_eye{ CAMERA_TIGER_EYE };
};

