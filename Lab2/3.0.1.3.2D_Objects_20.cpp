#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <windows.h>

#include <vector>    
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

enum WeaponMode { SWORD_MODE, GUN_MODE };
enum GameState { PLAYING, GAME_OVER, GAME_CLEAR };

WeaponMode current_mode = SWORD_MODE;
GameState game_state = PLAYING;

glm::vec2 player_pos(0.0f, 0.0f);
float player_speed = 3.0f;
float player_scale = 1.0f;

glm::vec2 hat_pos(0.0f, 0.0f);
bool key_pressed[256] = { false };

int score = 0;
int frame_counter = 0;
int elapsed_time = 0;
const int survive_time = 60;

bool bullet_active = false;
glm::vec2 bullet_pos;
glm::vec2 bullet_dir;
float bullet_speed = 10.0f;

float orbit_radius = 20.0f;      
float orbit_speed = 1.0f;        
float bullet_tick = 0.0f;        

float sword_angle = 0.0f;

struct Zombie {
	glm::vec2 pos;
	float angle;
	bool alive;
	int pattern;       
	glm::vec2 origin;   
	float tick;         
	glm::vec2 velocity;  
};
std::vector<Zombie> zombies;

struct SwordItem {
	glm::vec2 pos;
	bool collected = false;
};
std::vector<SwordItem> sword_items;  

struct GunItem {
	glm::vec2 pos;
	bool collected = false;
	bool visible = true;
};
GunItem gun_item;

bool has_gun = false;

void update_player();
void update_bullet();
void update_zombies();
void check_collisions();
bool is_colliding(glm::vec2 a, float ar, glm::vec2 b, float br);

void display_zombie_game();
void keyboard_down(unsigned char key, int x, int y);
void keyboard_up(unsigned char key, int x, int y);
void mouse_click(int button, int state, int x, int y);
void mouse_motion(int x, int y);
void timer_scene(int value);
void timer_game(int value);
void init_zombies();
void init_items();

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

// draw airplane
#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// hat
#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};

GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

// cake
#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -0.5, 14.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 0.5, 14.0 } };
GLfloat cake_candle[4][2] = { { -1.0, 8.0 } ,{ -1.0, 13.0 },{ 1.0, 13.0 },{ 1.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 5.0 },{ 10.0, 5.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 204 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 102 / 255.0f, 51 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}

// sword

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

// 1. star
#define NUM_STARS 20

struct Star {
	glm::vec2 center;      
	float angle_offset;    
	float distance;        
	float rotation_speed;  
};

std::vector<Star> stars;
float star_global_angle = 0.0f; 

GLfloat star_vertices[11][2] = {
	{0.0f, 0.0f},      
	{0.0f, 10.0f},     
	{2.5f, 2.5f},      
	{10.0f, 2.0f},     
	{4.0f, -2.0f},     
	{6.0f, -10.0f},    
	{0.0f, -4.0f},     
	{-6.0f, -10.0f},   
	{-4.0f, -2.0f},    
	{-10.0f, 2.0f},    
	{-2.5f, 2.5f}      
};
GLfloat star_color[3] = { 1.0f, 1.0f, 0.0f }; 

GLuint VBO_star, VAO_star;

void prepare_star() {
	glGenBuffers(1, &VBO_star);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_star);
	glBufferData(GL_ARRAY_BUFFER, sizeof(star_vertices), star_vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO_star);
	glBindVertexArray(VAO_star);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_star);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void init_stars() {
	stars.clear();
	float base_distance = 50.0f;

	for (int i = 0; i < NUM_STARS; ++i) {
		Star s;
		s.center = glm::vec2(0.0f, 0.0f);  
		s.angle_offset = 360.0f * i / NUM_STARS;
		s.distance = base_distance + i * 20.0f;  
		s.rotation_speed = 0.1f;  
		stars.push_back(s);
	}
}

void update_background_stars() {
	star_global_angle += 0.1f;  
	if (star_global_angle > 360.0f) star_global_angle -= 360.0f;
}

void draw_star(glm::vec2 pos) {
	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * M;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
	glUniform3fv(loc_primitive_color, 1, star_color);
	glBindVertexArray(VAO_star);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 11);
	glBindVertexArray(0);
}

// 2. butterfly
#define NUM_BUTTERFLIES 5
#define BUTTERFLY_LEFT_WING  0
#define BUTTERFLY_RIGHT_WING 1
#define BUTTERFLY_BODY       2

GLfloat left_wing[4][2] = {
	{ 0.0f, 0.0f }, {-20.0f, 30.0f}, {-40.0f, 10.0f}, {-20.0f, -20.0f}
};
GLfloat right_wing[4][2] = {
	{ 0.0f, 0.0f }, {20.0f, 30.0f}, {40.0f, 10.0f}, {20.0f, -20.0f}
};
GLfloat butterfly_body[3][2] = {
	{ -5.0f, -15.0f }, { 0.0f, 15.0f }, { 5.0f, -15.0f }
};
GLfloat butterfly_color[3][3] = {
	{ 1.0f, 0.0f, 0.0f },  
	{ 0.0f, 1.0f, 0.0f },  
	{ 0.0f, 0.0f, 0.0f }   
};

GLuint VBO_butterfly, VAO_butterfly;

struct Butterfly {
	glm::vec2 center;
	float angle;
	float radius_x;
	float radius_y;
	float angle_speed;
	float wing_flap;
	glm::vec2 pos; 
};
std::vector<Butterfly> butterflies;

void prepare_butterfly() {
	GLsizeiptr buffer_size = sizeof(left_wing) + sizeof(right_wing) + sizeof(butterfly_body);
	glGenBuffers(1, &VBO_butterfly);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_butterfly);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_wing), left_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_wing), sizeof(right_wing), right_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_wing) + sizeof(right_wing), sizeof(butterfly_body), butterfly_body);

	glGenVertexArrays(1, &VAO_butterfly);
	glBindVertexArray(VAO_butterfly);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_butterfly);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void init_butterflies() {
	butterflies.clear();
	for (int i = 0; i < NUM_BUTTERFLIES; ++i) {
		Butterfly b;
		b.center = glm::vec2(
			glm::linearRand(-300.0f, 300.0f),
			glm::linearRand(-200.0f, 200.0f)
		);
		b.angle = glm::linearRand(0.0f, 360.0f);
		b.radius_x = glm::linearRand(100.0f, 250.0f);
		b.radius_y = glm::linearRand(80.0f, 180.0f);
		b.angle_speed = glm::linearRand(0.3f, 0.8f);
		b.wing_flap = 0.0f;
		butterflies.push_back(b);
	}
}

void update_butterflies() {
	int time_ms = glutGet(GLUT_ELAPSED_TIME);
	float time = time_ms * 0.001f; 

	for (int i = 0; i < butterflies.size(); ++i) {
		auto& b = butterflies[i];

		b.angle += b.angle_speed;
		if (b.angle > 360.0f) b.angle -= 360.0f;

		float rad = b.angle * TO_RADIAN;

		float wiggle_x = 10.0f * sin(time * 2.0f + i);  
		float wiggle_y = 10.0f * cos(time * 3.0f + i);  

		b.pos.x = b.center.x + b.radius_x * cos(rad) + wiggle_x;
		b.pos.y = b.center.y + b.radius_y * sin(rad) + wiggle_y;

		b.wing_flap = sin(time_ms * 0.01f + rad) * 20.0f;
	}
}

void draw_butterflies() {
	glBindVertexArray(VAO_butterfly);

	for (auto& b : butterflies) {
		glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(b.pos, 0.0f));

		glm::mat4 ML = glm::rotate(base, TO_RADIAN * b.wing_flap, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix * ML));
		glUniform3fv(loc_primitive_color, 1, butterfly_color[BUTTERFLY_LEFT_WING]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glm::mat4 MR = glm::rotate(base, TO_RADIAN * -b.wing_flap, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix * MR));
		glUniform3fv(loc_primitive_color, 1, butterfly_color[BUTTERFLY_RIGHT_WING]);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ViewProjectionMatrix * base));
		glUniform3fv(loc_primitive_color, 1, butterfly_color[BUTTERFLY_BODY]);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 3);
	}

	glBindVertexArray(0);
}

void display_zombie_game() {
	glClear(GL_COLOR_BUFFER_BIT);

	ProjectionMatrix = glm::ortho(
		-win_width / 2.0f, win_width / 2.0f,
		-win_height / 2.0f, win_height / 2.0f,
		-1000.0f, 1000.0f
	);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glm::mat4 ModelMatrix;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(player_pos, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(player_scale, player_scale, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
	draw_airplane();

	if (current_mode == GUN_MODE && game_state == PLAYING && has_gun) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(hat_pos, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		draw_hat();
	}

	if (current_mode == SWORD_MODE) {
		int num_swords = 0;
		for (auto& item : sword_items)
			if (item.collected)
				num_swords++;

		for (int i = 0; i < num_swords; ++i) {
			float angle_deg = sword_angle + (360.0f / num_swords) * i;
			float angle_rad = angle_deg * TO_RADIAN;

			glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(player_pos, 0.0f));
			M = glm::rotate(M, angle_rad, glm::vec3(0, 0, 1));
			M = glm::translate(M, glm::vec3(40.0f, 0.0f, 0.0f));

			ModelViewProjectionMatrix = ViewProjectionMatrix * M;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
			draw_sword();
		}
	}

	if (current_mode == GUN_MODE && bullet_active) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(bullet_pos, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		draw_cake();

		int num_orbit = 3;  
		for (int i = 0; i < num_orbit; ++i) {
			float angle = bullet_tick + i * (360.0f / num_orbit);
			float rad = angle * TO_RADIAN;
			float orbit_radius = 20.0f;

			glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(bullet_pos, 0.0f));  
			M = glm::rotate(M, rad, glm::vec3(0, 0, 1));                                 
			M = glm::translate(M, glm::vec3(orbit_radius, 0.0f, 0.0f));                  

			ModelViewProjectionMatrix = ViewProjectionMatrix * M;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
			draw_cake();
		}
	}

	for (auto& z : zombies) {
		if (!z.alive) continue;
		float scale = 1.0f;
		if (z.pattern == 5) { 
			scale = 1.0f + 1.3f * sin(z.tick);
		}
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(z.pos, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale, scale, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		draw_car2();
	}

	for (auto& item : sword_items) {
		if (item.collected) continue;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(item.pos, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		draw_sword();
	}

	if (!gun_item.collected && gun_item.visible) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(gun_item.pos, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		draw_hat();
	}

	for (auto& s : stars) {
		float angle = star_global_angle + s.angle_offset;
		float rad = angle * TO_RADIAN;
		glm::vec2 pos = s.center + glm::vec2(cos(rad), sin(rad)) * s.distance;
		draw_star(pos);
	}

	update_butterflies();  
	draw_butterflies();

	glFlush();
}

void update_player() {
	if (game_state != PLAYING) return;

	if (key_pressed['w'] || key_pressed['W']) player_pos.y += player_speed;
	if (key_pressed['s'] || key_pressed['S']) player_pos.y -= player_speed;
	if (key_pressed['a'] || key_pressed['A']) player_pos.x -= player_speed;
	if (key_pressed['d'] || key_pressed['D']) player_pos.x += player_speed;

	float half_w = win_width / 2.0f;
	float half_h = win_height / 2.0f;
	player_pos.x = glm::clamp(player_pos.x, -half_w, half_w);
	player_pos.y = glm::clamp(player_pos.y, -half_h, half_h);

	if (current_mode == SWORD_MODE) {
		int num_swords = 0;
		for (auto& item : sword_items)
			if (item.collected) num_swords++;

		if (num_swords > 0)
			sword_angle += 3.0f; 
	}

	// 플레이어 크기 조정 (60초 주기로 삼각 파형)
	float t = fmod(elapsed_time, 60.0f);
	if (t < 30.0f)
		player_scale = 1.0f + (2.0f * (t / 30.0f)); // 1.0 ~ 3.0
	else
		player_scale = 3.0f - (2.0f * ((t - 30.0f) / 30.0f)); // 3.0 ~ 1.0
}

void update_bullet() {
	if (game_state != PLAYING || !bullet_active) return;

	bullet_pos += bullet_dir * bullet_speed;

	bullet_tick += 10.0f; 
	if (bullet_tick >= 360.0f)
		bullet_tick -= 360.0f;

	float half_w = win_width / 2.0f;
	float half_h = win_height / 2.0f;
	if (bullet_pos.x < -half_w || bullet_pos.x > half_w || bullet_pos.y < -half_h || bullet_pos.y > half_h)
		bullet_active = false;
}

void update_zombies() {
	if (game_state != PLAYING) return;

	for (auto& z : zombies) {
		if (!z.alive) continue;
		z.tick += 0.1f;

		switch (z.pattern) {
		case 0: { 
			glm::vec2 dir = glm::normalize(player_pos - z.pos);
			z.pos += dir * 1.5f;
			break;
		}
		case 1: { 
			z.pos.x = z.origin.x + 50.0f * sin(z.tick);
			break;
		}
		case 2: { 
			z.pos.y = z.origin.y + 50.0f * sin(z.tick);
			break;
		}
		case 3: {
			z.pos.x = z.origin.x + 50.0f * cos(z.tick);
			z.pos.y = z.origin.y + 50.0f * sin(z.tick);
			break;
		}
		case 4: { 
			z.pos += z.velocity;
			float half_w = win_width / 2.0f;
			float half_h = win_height / 2.0f;
			if (z.pos.x > half_w + 50.0f || z.pos.y < -half_h - 50.0f) {
				z.pos = glm::vec2(-half_w - 50.0f, half_h + 50.0f); 
			}
			break;
		}
		case 5: { 
			z.pos.x += 3.0f;
			if (z.pos.x > win_width / 2.0f)
				z.pos.x = -win_width / 2.0f;
			break;
		}
		case 6: {
			float scale = 1.0f + 0.5f * sin(z.tick * 2.0f);
			z.angle = scale; 
			break;
		}
		case 7: { 
			float amplitude = 40.0f;
			float frequency = 5.0f;
			z.pos.x = z.origin.x + amplitude * sin(z.tick * frequency);
			break;
		}
		case 8: { 
			float dx = (rand() % 200 - 100) / 100.0f; 
			float dy = (rand() % 200 - 100) / 100.0f;
			glm::vec2 random_dir = glm::normalize(glm::vec2(dx, dy));
			z.pos += random_dir * 2.0f;
			break;
		}
		}
	}
}

void reset_game() {
	player_pos = glm::vec2(0.0f, 0.0f);
	hat_pos = player_pos;
	bullet_active = false;
	score = 0;
	elapsed_time = 0;
	frame_counter = 0;
	game_state = PLAYING;
	current_mode = SWORD_MODE;
	has_gun = false;

	sword_angle = 0.0f;
	sword_items.clear();

	gun_item.collected = false;
	gun_item.visible = true;

	init_zombies();

	init_items();

	init_stars();
	prepare_star();

	init_butterflies();

	printf("===> Game restarted!\n");
}

void check_collisions() {
	if (game_state != PLAYING) return;

	for (auto& z : zombies) {
		if (!z.alive) continue;

		float player_radius = 20.0f * player_scale;

		float zombie_scale = 1.0f;
		if (z.pattern == 5)
			zombie_scale = 1.0f + 1.3f * sin(z.tick);
		else if (z.pattern == 6)
			zombie_scale = z.angle;  
		float zombie_radius = 20.0f * zombie_scale;

		if (is_colliding(player_pos, player_radius, z.pos, zombie_radius)) {
			game_state = GAME_OVER;
			printf("===> GAME OVER! Final Score: %d, Time: %d seconds\n", score, elapsed_time);
			return;
		}

		if (current_mode == SWORD_MODE) {
			int num_swords = 0;
			for (auto& item : sword_items)
				if (item.collected)
					num_swords++;

			for (int i = 0; i < num_swords; ++i) {
				float angle = sword_angle + (360.0f / num_swords) * i;
				float rad = angle * TO_RADIAN;
				glm::vec2 sword_tip = player_pos + glm::vec2(cos(rad), sin(rad)) * 40.0f;

				if (is_colliding(z.pos, zombie_radius, sword_tip, 10.0f)) {
					z.alive = false;
					score += 2;
					break;
				}
			}
		}

		if (current_mode == GUN_MODE && bullet_active) {
			if (is_colliding(z.pos, 20.0f, bullet_pos, 10.0f)) {
				z.alive = false;
				bullet_active = false;
				score++;
				continue;
			}

			int num_orbit = 3;
			for (int i = 0; i < num_orbit; ++i) {
				float angle = bullet_tick + i * (360.0f / num_orbit);
				float rad = angle * TO_RADIAN;
				glm::vec2 orbit_pos = bullet_pos + glm::vec2(cos(rad), sin(rad)) * 20.0f;

				if (is_colliding(z.pos, 20.0f, orbit_pos, 10.0f)) {
					z.alive = false;
					bullet_active = false;
					score++;
					break;
				}
			}
		}
	}

	for (auto& item : sword_items) {
		if (!item.collected && is_colliding(player_pos, 20.0f * player_scale, item.pos, 10.0f)) {
			item.collected = true;
			printf("===> Sword acquired!\n");
		}
	}

	if (!gun_item.collected && gun_item.visible &&
		is_colliding(player_pos, 20.0f * player_scale, gun_item.pos, 10.0f)) {
		gun_item.collected = true;
		has_gun = true;
		printf("===> Gun acquired!\n");
	}

	bool all_dead = true;
	for (auto& z : zombies) {
		if (z.alive) {
			all_dead = false;
			break;
		}
	}
	if (all_dead) {
		game_state = GAME_CLEAR;
		printf("===> CLEAR! All zombies defeated. Score: %d, Time: %d seconds\n", score, elapsed_time);
	}
}

bool is_colliding(glm::vec2 p1, float r1, glm::vec2 p2, float r2) {
	return glm::length(p1 - p2) < (r1 + r2);
}

void timer_scene(int value) {
	if (game_state == PLAYING) {
		update_player();
		update_bullet();
		update_zombies();
		update_background_stars();
		update_butterflies();
		check_collisions();
		glutPostRedisplay();
	}
	glutTimerFunc(16, timer_scene, 0);
}

void timer_game(int value) {
	if (game_state == PLAYING) {
		update_player();
		update_zombies();
		update_bullet();
		check_collisions();

		frame_counter++;
		if (frame_counter >= 60) { 
			frame_counter = 0;
			elapsed_time++;

			if (elapsed_time % 10 == 0) {
				for (auto& z : zombies) {
					if (z.pattern == 0) {
						float safe_distance = 150.0f;
						int attempts = 0;
						glm::vec2 new_pos;
						do {
							new_pos = glm::vec2(glm::linearRand(-win_width / 2.0f + 50.0f, win_width / 2.0f - 50.0f),
								glm::linearRand(-win_height / 2.0f + 50.0f, win_height / 2.0f - 50.0f));
							attempts++;
						} while (glm::distance(new_pos, player_pos) < safe_distance && attempts < 100);

						z.pos = new_pos;
						z.origin = new_pos;
						z.tick = 0.0f;
					}
				}
			}

			bool all_dead = true;
			for (auto& z : zombies)
				if (z.alive) {
					all_dead = false;
					break;
				}
			if (all_dead) {
				game_state = GAME_CLEAR;
				printf("===> CLEAR! All zombies eliminated. Time: %d seconds. Score: %d\n", elapsed_time, score);
			}

			if (elapsed_time >= survive_time) {
				game_state = GAME_OVER;
				printf("===> GAME OVER! Time limit exceeded. Score: %d, Time: %d seconds\n", score, elapsed_time);
				return; 
			}

			printf("TIME: %ds   SCORE: %d\n", elapsed_time, score);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(16, timer_game, 0);
}

void keyboard_down(unsigned char key, int x, int y) {
	key_pressed[key] = true;

	switch (key) {
	case 27: // ESC
		glutLeaveMainLoop();
		break;
	case 'z': case 'Z':
		current_mode = SWORD_MODE;
		break;
	case 'x': case 'X':
		current_mode = GUN_MODE;
		break;
	case 'r': case 'R':
		reset_game();
		break;
	}
}

void keyboard_up(unsigned char key, int x, int y) {
	key_pressed[key] = false;
}

void mouse_click(int button, int state, int x, int y) {
	if (game_state != PLAYING) return;

	if (current_mode == GUN_MODE && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		bullet_active = true;
		bullet_pos = player_pos;
		bullet_dir = glm::normalize(hat_pos - player_pos);
	}
}

void mouse_motion(int x, int y) {
	if (game_state != PLAYING) return;
	hat_pos.x = x - win_width / 2.0f;
	hat_pos.y = win_height / 2.0f - y;
}

void init_zombies() {
	zombies.clear();
	int num_chasing_zombies = 15; 
	int num_other_zombies = 15;   

	float safe_distance = 150.0f;
	int max_attempts = 100;

	for (int i = 0; i < num_chasing_zombies; ++i) {
		glm::vec2 pos;
		int attempts = 0;

		do {
			float margin = 100.0f; 
			pos = glm::vec2(
				glm::linearRand(-win_width / 2.0f + margin, win_width / 2.0f - margin),
				glm::linearRand(-win_height / 2.0f + margin, win_height / 2.0f - margin)
			);
			attempts++;
		} while (glm::distance(pos, player_pos) < safe_distance && attempts < max_attempts);

		zombies.push_back({ pos, 0.0f, true, 0, pos, 0.0f });
	}

	for (int i = 0; i < num_other_zombies; ++i) {
		glm::vec2 pos;
		int attempts = 0;

		do {
			float margin = 50.0f; 
			pos = glm::vec2(
				glm::linearRand(-win_width / 2.0f + margin, win_width / 2.0f - margin),
				glm::linearRand(-win_height / 2.0f + margin, win_height / 2.0f - margin)
			);
			attempts++;
		} while (glm::distance(pos, player_pos) < safe_distance && attempts < max_attempts);

		int pattern = glm::linearRand(1, 8);
		zombies.push_back({ pos, 0.0f, true, pattern, pos, 0.0f });
	}
}

void init_items() {
	sword_items.clear();
	float safe_distance = 150.0f;

	for (int i = 0; i < 3; ++i) {
		glm::vec2 pos;
		int attempts = 0;

		do {
			pos = glm::vec2(glm::linearRand(-win_width / 2.0f + 50.0f, win_width / 2.0f - 50.0f),
				glm::linearRand(-win_height / 2.0f + 50.0f, win_height / 2.0f - 50.0f));
			attempts++;
		} while (glm::distance(pos, player_pos) < safe_distance && attempts < 100);

		sword_items.push_back({ pos, false });
	}

	glm::vec2 gun_pos;
	int gun_attempts = 0;

	do {
		gun_pos = glm::vec2(glm::linearRand(-win_width / 2.0f + 50.0f, win_width / 2.0f - 50.0f),
			glm::linearRand(-win_height / 2.0f + 50.0f, win_height / 2.0f - 50.0f));
		gun_attempts++;
	} while (glm::distance(gun_pos, player_pos) < safe_distance && gun_attempts < 100);

	gun_item.pos = gun_pos;
	gun_item.collected = false;
	gun_item.visible = true;

	has_gun = false; 
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

}

void register_callbacks(void) {
	glutDisplayFunc(display_zombie_game);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(100 / 255.0f, 90 / 255.0f, 40 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_airplane();
	prepare_car2();
	prepare_hat();
	prepare_cake();
	prepare_sword();
	prepare_butterfly();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
int main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_ZombieGame";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC', 'WASD', 'Z', 'X', mouse move, click"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);  // 너비 증가
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	// Zombie Game용 콜백 등록
	glutDisplayFunc(display_zombie_game);
	glutKeyboardFunc(keyboard_down);
	glutKeyboardUpFunc(keyboard_up);
	glutMouseFunc(mouse_click);
	glutPassiveMotionFunc(mouse_motion);
	glutTimerFunc(0, timer_scene, 0);
	glutTimerFunc(1000, timer_game, 0);

	glutTimerFunc(100, [](int) {
		reset_game();
		}, 0);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}



