#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CCW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Wolf_D::define_object() {
#define N_WOLF_FRAMES 17
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_WOLF_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/wolf/wolf_%d%d_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CCW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::mat4(1.0f);
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.00f, 0.00f, 0.00f, 1.0f);
		cur_material->ambient = glm::vec4(0.10f, 0.10f, 0.10f, 1.0f);
		cur_material->diffuse = glm::vec4(0.70f, 0.70f, 0.70f, 1.0f);
		cur_material->specular = glm::vec4(0.50f, 0.50f, 0.50f, 1.0f);
		cur_material->exponent = 32.0f;
	}
}

void Cow_D::define_object() {
#define N_FRAMES_COW_1 1
#define N_FRAMES_COW_2 1
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;
	switch (object_id) {

		int n_frames;
	case DYNAMIC_OBJECT_COW_1:
		n_frames = N_FRAMES_COW_1;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();
			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
			cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
			cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
			cur_material->exponent = 128.0f * 0.21794872f;
		}
		break;
	case DYNAMIC_OBJECT_COW_2:
		n_frames = N_FRAMES_COW_2;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
			cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
			cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
			cur_material->exponent = 128.0f * 0.6f;
		}
		break;
	}
}

void Dynamic_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	static bool tigerInit = false;
	static std::vector<glm::vec2> waypoints;
	static std::vector<float> segmentLengths;
	static float totalLoopLength;
	static float speedTiger = 100.0f * TO_RADIAN;

	if (!tigerInit) {
		tigerInit = true;

		waypoints.clear();
		waypoints.push_back(glm::vec2(45.0f, 140.0f));  
		waypoints.push_back(glm::vec2(35.0f, 140.0f));  
		waypoints.push_back(glm::vec2(23.0f, 115.0f));  
		waypoints.push_back(glm::vec2(23.0f, 100.0f));  
		waypoints.push_back(glm::vec2(35.0f, 85.0f));   
		waypoints.push_back(glm::vec2(75.0f, 80.0f));   
		waypoints.push_back(glm::vec2(90.0f, 97.0f));   
		waypoints.push_back(glm::vec2(200.0f, 97.0f));  
		waypoints.push_back(glm::vec2(210.0f, 90.0f));  
		waypoints.push_back(glm::vec2(210.0f, 60.0f));  
		waypoints.push_back(glm::vec2(200.0f, 50.0f));  
		waypoints.push_back(glm::vec2(185.0f, 40.0f));  
		waypoints.push_back(glm::vec2(185.0f, 30.0f));  
		waypoints.push_back(glm::vec2(190.0f, 25.0f));  
		waypoints.push_back(glm::vec2(210.0f, 25.0f));  

		waypoints.push_back(glm::vec2(190.0f, 25.0f));  
		waypoints.push_back(glm::vec2(185.0f, 30.0f));  
		waypoints.push_back(glm::vec2(185.0f, 40.0f));  
		waypoints.push_back(glm::vec2(200.0f, 50.0f));  
		waypoints.push_back(glm::vec2(210.0f, 60.0f));  
		waypoints.push_back(glm::vec2(210.0f, 90.0f));  
		waypoints.push_back(glm::vec2(200.0f, 97.0f));  
		waypoints.push_back(glm::vec2(90.0f, 97.0f));   
		waypoints.push_back(glm::vec2(75.0f, 80.0f));   
		waypoints.push_back(glm::vec2(35.0f, 85.0f));   
		waypoints.push_back(glm::vec2(23.0f, 100.0f));  
		waypoints.push_back(glm::vec2(23.0f, 115.0f));  
		waypoints.push_back(glm::vec2(35.0f, 140.0f));  
		waypoints.push_back(glm::vec2(45.0f, 140.0f));  

		int N = (int)waypoints.size();
		segmentLengths.resize(N - 1);
		totalLoopLength = 0.0f;
		for (int i = 0; i < N - 1; ++i) {
			float dx = waypoints[i + 1].x - waypoints[i].x;
			float dy = waypoints[i + 1].y - waypoints[i].y;
			float len = std::sqrt(dx * dx + dy * dy);
			segmentLengths[i] = len;
			totalLoopLength += len;
		}
	}

	static bool wolfInit = false;
	static std::vector<glm::vec2> wolfWaypoints;
	static std::vector<float> wolfSegmentLengths;
	static float wolfTotalLoop;
	static float speedWolf = 250.0f * TO_RADIAN;

	if (!wolfInit) {
		wolfInit = true;

		wolfWaypoints.clear();
		wolfWaypoints.push_back(glm::vec2(30.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(65.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(80.0f, 35.0f)); 
		wolfWaypoints.push_back(glm::vec2(80.0f, 90.0f)); 
		wolfWaypoints.push_back(glm::vec2(95.0f, 105.0f)); 
		wolfWaypoints.push_back(glm::vec2(100.0f, 110.0f)); 
		wolfWaypoints.push_back(glm::vec2(153.0f, 110.0f)); 
		wolfWaypoints.push_back(glm::vec2(160.0f, 100.0f)); 
		wolfWaypoints.push_back(glm::vec2(205.0f, 100.0f)); 
		wolfWaypoints.push_back(glm::vec2(220.0f, 120.0f)); 
		wolfWaypoints.push_back(glm::vec2(220.0f, 145.0f)); 
		wolfWaypoints.push_back(glm::vec2(198.0f, 145.0f)); 

		wolfWaypoints.push_back(glm::vec2(220.0f, 145.0f)); 
		wolfWaypoints.push_back(glm::vec2(220.0f, 120.0f)); 
		wolfWaypoints.push_back(glm::vec2(205.0f, 100.0f)); 
		wolfWaypoints.push_back(glm::vec2(160.0f, 100.0f)); 
		wolfWaypoints.push_back(glm::vec2(153.0f, 110.0f)); 

		wolfWaypoints.push_back(glm::vec2(125.0f, 100.0f)); 
		wolfWaypoints.push_back(glm::vec2(125.0f, 80.0f)); 
		wolfWaypoints.push_back(glm::vec2(130.0f, 70.0f)); 
		wolfWaypoints.push_back(glm::vec2(150.0f, 63.0f)); 
		wolfWaypoints.push_back(glm::vec2(115.0f, 63.0f)); 
		wolfWaypoints.push_back(glm::vec2(110.0f, 50.0f)); 
		wolfWaypoints.push_back(glm::vec2(110.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(150.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(110.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(110.0f, 50.0f)); 
		wolfWaypoints.push_back(glm::vec2(115.0f, 63.0f)); 
		wolfWaypoints.push_back(glm::vec2(125.0f, 85.0f)); 
		wolfWaypoints.push_back(glm::vec2(115.0f, 100.0f)); 

		wolfWaypoints.push_back(glm::vec2(100.0f, 110.0f)); 
		wolfWaypoints.push_back(glm::vec2(95.0f, 105.0f)); 
		wolfWaypoints.push_back(glm::vec2(80.0f, 90.0f)); 
		wolfWaypoints.push_back(glm::vec2(80.0f, 35.0f)); 
		wolfWaypoints.push_back(glm::vec2(65.0f, 20.0f)); 
		wolfWaypoints.push_back(glm::vec2(30.0f, 20.0f)); 

		int N = (int)wolfWaypoints.size();
		wolfSegmentLengths.resize(N - 1);
		wolfTotalLoop = 0.0f;
		for (int i = 0; i < N - 1; ++i) {
			float dx = wolfWaypoints[i + 1].x - wolfWaypoints[i].x;
			float dy = wolfWaypoints[i + 1].y - wolfWaypoints[i].y;
			float len = std::sqrt(dx * dx + dy * dy);
			wolfSegmentLengths[i] = len;
			wolfTotalLoop += len;
		}
	}

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	switch (object_id) {
	case DYNAMIC_OBJECT_TIGER: {
		float rawDist = (float)time_stamp * speedTiger;
		float curDist = std::fmod(rawDist, totalLoopLength);

		float accumulated = 0.0f;
		int segmentIdx = 0;
		float segT = 0.0f;

		int M = (int)segmentLengths.size();
		for (int i = 0; i < M; ++i) {
			if (curDist <= accumulated + segmentLengths[i]) {
				segmentIdx = i;
				float distInto = curDist - accumulated;
				segT = distInto / segmentLengths[i]; 
				break;
			}
			accumulated += segmentLengths[i];
		}

		glm::vec2 Pa = waypoints[segmentIdx];
		glm::vec2 Pb = waypoints[segmentIdx + 1];

		float curX = Pa.x + (Pb.x - Pa.x) * segT;
		float curY = Pa.y + (Pb.y - Pa.y) * segT;
		float dx = Pb.x - Pa.x;
		float dy = Pb.y - Pa.y;
		float segLen = segmentLengths[segmentIdx];
		float dirX = dx / segLen;
		float dirY = dy / segLen;
		float angleRad = std::atan2(dirY, dirX);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(curX, curY, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, angleRad + 33.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		{
			glm::vec4 eyeLocalPos(0.0f, -17.0f, 13.0f, 1.0f);
			glm::vec4 eyeWorldPos4 = ModelMatrix * eyeLocalPos;
			glm::vec3 eyeWorldPos(eyeWorldPos4.x, eyeWorldPos4.y, eyeWorldPos4.z);

			glm::vec3 tigerForward = glm::normalize(glm::vec3(
				ModelMatrix * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)
			));

			glm::vec3 upVec(0.0f, 0.0f, 1.0f);

			Perspective_Camera& tigerCam =
				static_cast<Perspective_Camera&>(
					scene.camera_list[camera_ID_mapper[CAMERA_TIGER_EYE]].get()
					);

			tigerCam.cam_view.pos = eyeWorldPos;
			tigerCam.cam_view.naxis = tigerForward;
			tigerCam.cam_view.vaxis = upVec;
			tigerCam.cam_view.uaxis = glm::normalize(glm::cross(upVec, tigerForward));

			glm::vec3 center = eyeWorldPos + tigerForward;
			tigerCam.ViewMatrix = glm::lookAt(eyeWorldPos, center, upVec);
		}

		break;
	}
	case DYNAMIC_OBJECT_COW_1:
		rotation_angle = (2 * time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	case DYNAMIC_OBJECT_COW_2:
		rotation_angle = (5 * time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 50.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case DYNAMIC_OBJECT_WOLF: {
		float rawDist = (float)time_stamp * speedWolf;
		float curDist = std::fmod(rawDist, wolfTotalLoop);

		float accumulated = 0.0f;
		int segmentIdx = 0;
		float segT = 0.0f;

		int M = (int)wolfSegmentLengths.size(); 
		for (int i = 0; i < M; ++i) {
			if (curDist < accumulated + wolfSegmentLengths[i]) {
				segmentIdx = i;
				float distInto = curDist - accumulated;
				segT = distInto / wolfSegmentLengths[i];
				break;
			}
			accumulated += wolfSegmentLengths[i];
		}

		glm::vec2 Pa = wolfWaypoints[segmentIdx];
		glm::vec2 Pb = wolfWaypoints[segmentIdx + 1];

		float curX = Pa.x + (Pb.x - Pa.x) * segT;
		float curY = Pa.y + (Pb.y - Pa.y) * segT;
		float dx = Pb.x - Pa.x;
		float dy = Pb.y - Pa.y;
		float segLen = wolfSegmentLengths[segmentIdx];
		float dirX = dx / segLen;
		float dirY = dy / segLen;
		float angleRad = std::atan2(dirY, dirX);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(curX, curY, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, angleRad + 33.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(28.0f, 28.0f, 28.0f));

		break;
	}
	}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}
