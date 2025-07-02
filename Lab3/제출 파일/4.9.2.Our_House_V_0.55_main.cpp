#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

static Camera_ID activeCameraID = CAMERA_MAIN;
static bool showOrtho = false;
static bool showCameraFrames = false;
static bool showTigerEye = false;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& refCam : scene.camera_list) {
        Camera& cam = refCam.get();
        if (!cam.flag_valid) continue;

        bool renderThis = false;
        if (showOrtho) {
            if (cam.camera_id == CAMERA_MAIN
                || cam.camera_id == CAMERA_SIDE_FRONT
                || cam.camera_id == CAMERA_TOP
                || cam.camera_id == CAMERA_SIDE)
                renderThis = true;
        }
        else {
            if (cam.camera_id == CAMERA_MAIN
                || cam.camera_id == CAMERA_DYN_CCTV
                || cam.camera_id == CAMERA_CC_0
                || cam.camera_id == CAMERA_CC_1
                || cam.camera_id == CAMERA_CC_2)
                renderThis = true;
        }
        if (!renderThis) continue;

        glViewport(
            (int)cam.view_port.x,
            (int)cam.view_port.y,
            (int)cam.view_port.w,
            (int)cam.view_port.h
        );

        scene.currentDrawingCamera = cam.camera_id;
        scene.ViewMatrix = cam.ViewMatrix;
        scene.ProjectionMatrix = cam.ProjectionMatrix;

        scene.draw_world();

        if (showCameraFrames) {
            glDisable(GL_DEPTH_TEST);

            if (cam.camera_id == CAMERA_MAIN) {
                for (auto& innerRef : scene.camera_list) {
                    Camera& other = innerRef.get();
                    if (!other.flag_valid) continue;
                    if (other.camera_id == CAMERA_MAIN) continue;
                    if (other.camera_id == CAMERA_SIDE_FRONT ||
                        other.camera_id == CAMERA_TOP ||
                        other.camera_id == CAMERA_SIDE)
                        continue;

                    scene.draw_camera_frame(other);
                }
            }
            else {
                for (auto& innerRef : scene.camera_list) {
                    Camera& other = innerRef.get();
                    if (!other.flag_valid)            continue;
                    if (other.camera_id == cam.camera_id) continue;
                    if (other.camera_id == CAMERA_SIDE_FRONT ||
                        other.camera_id == CAMERA_TOP ||
                        other.camera_id == CAMERA_SIDE)
                        continue;

                    scene.draw_camera_frame(other);
                }
            }
            glEnable(GL_DEPTH_TEST);
        }
    }

    if (showTigerEye) {
        Perspective_Camera& tigerCam =
            static_cast<Perspective_Camera&>(
                scene.camera_list[camera_ID_mapper[CAMERA_TIGER_EYE]].get()
                );
        if (tigerCam.flag_valid) {
            glViewport((int)tigerCam.view_port.x, (int)tigerCam.view_port.y,
                (int)tigerCam.view_port.w, (int)tigerCam.view_port.h);
            scene.currentDrawingCamera = tigerCam.camera_id;
            scene.ViewMatrix = tigerCam.ViewMatrix;
            scene.ProjectionMatrix = tigerCam.ProjectionMatrix;
            scene.draw_world();
        }
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'f' || key == 'F') {
        showOrtho = !showOrtho;
        printf(">>> Now %s mode: %s\n",
            (showOrtho ? "ORTHO" : "CAMERA+CCTV"),
            (showOrtho
                ? "Main + 정면도/측면도/상면도"
                : "Main + 정적 CCTV 0/1/2 + 동적 CCTV"));
        glutPostRedisplay();
        return;
    }
    else if (key == 't' || key == 'T') {
        showCameraFrames = !showCameraFrames;
        printf(">>> Camera Frames %s\n", showCameraFrames ? "ON" : "OFF");
        glutPostRedisplay();
        return;
    }

    switch (key) {
    case '1':
        activeCameraID = CAMERA_MAIN;
        printf("▶▶ Active Camera: MAIN\n");
        return; 
    case '2':
        activeCameraID = CAMERA_DYN_CCTV;
        printf("▶▶ Active Camera: DYNAMIC CCTV\n");
        return;
    case '3':
        showTigerEye = !showTigerEye;
        printf(">>> Tiger Eye Camera %s\n", showTigerEye ? "ON" : "OFF");
        glutPostRedisplay();
        return;
    default:
        break;
    }

    static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
    Perspective_Camera& mainCam = static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_MAIN]].get());
    Perspective_Camera& dCctv = static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_DYN_CCTV]].get());

    const float moveSpeed = 10.0f;
    const float rotateSpeed = 1.0f * TO_RADIAN;
    Camera_ID target = activeCameraID;

    switch (key) {
    case 27: // ESC key
        glutLeaveMainLoop(); // Incur destuction callback for cleanups.
        break;
    case 'q': case 'Q': { 
        glm::vec3 forward = -mainCam.cam_view.naxis;
        mainCam.cam_view.pos += forward * moveSpeed;
        break;
    }
    case 'e': case 'E': { 
        glm::vec3 backward = mainCam.cam_view.naxis;
        mainCam.cam_view.pos += backward * moveSpeed;
        break;
    }
    case 'a': case 'A': { 
        glm::vec3 left = -mainCam.cam_view.uaxis;
        mainCam.cam_view.pos += left * moveSpeed;
        break;
    }
    case 'd': case 'D': { 
        glm::vec3 right = mainCam.cam_view.uaxis;
        mainCam.cam_view.pos += right * moveSpeed;
        break;
    }
    case 'w': case 'W': {
        glm::vec3 up = mainCam.cam_view.vaxis;
        mainCam.cam_view.pos += up * moveSpeed;
        break;
    }
    case 's': case 'S': {
        glm::vec3 down = -mainCam.cam_view.vaxis;
        mainCam.cam_view.pos += down * moveSpeed;
        break;
    }
    case 'z': case 'Z': {
        if (target == CAMERA_MAIN) {
            glm::vec3 axis = mainCam.cam_view.naxis;
            float angle = +(1.0f * TO_RADIAN);
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.uaxis, 0.0f)));
            mainCam.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.vaxis, 0.0f)));
        }
        else if (target == CAMERA_DYN_CCTV) {
            glm::vec3 axis = dCctv.cam_view.naxis;
            float angle = +(1.0f * TO_RADIAN);
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.uaxis, 0.0f)));
            dCctv.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.vaxis, 0.0f)));
        }
        break;
    }
    case 'x': case 'X': {
        if (target == CAMERA_MAIN) {
            glm::vec3 axis = mainCam.cam_view.naxis;
            float angle = -(1.0f * TO_RADIAN);
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.uaxis, 0.0f)));
            mainCam.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.vaxis, 0.0f)));
        }
        else if (target == CAMERA_DYN_CCTV) {
            glm::vec3 axis = dCctv.cam_view.naxis;
            float angle = -(1.0f * TO_RADIAN);
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.uaxis, 0.0f)));
            dCctv.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.vaxis, 0.0f)));
        }
        break;
    }
    case 'c': case 'C':
        flag_cull_face = (flag_cull_face + 1) % 3;
        switch (flag_cull_face) {
        case 0:
            glDisable(GL_CULL_FACE);
            glutPostRedisplay();
            fprintf(stdout, "^^^ No faces are culled.\n");
            break;
        case 1: // cull back faces;
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            glutPostRedisplay();
            fprintf(stdout, "^^^ Back faces are culled.\n");
            break;
        case 2: // cull front faces;
            glCullFace(GL_FRONT);
            glEnable(GL_CULL_FACE);
            glutPostRedisplay();
            fprintf(stdout, "^^^ Front faces are culled.\n");
            break;
        }
        break;
    case 'v': case 'V':
        polygon_fill_on = 1 - polygon_fill_on;
        if (polygon_fill_on) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            fprintf(stdout, "^^^ Polygon filling enabled.\n");
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            fprintf(stdout, "^^^ Line drawing enabled.\n");
        }
        glutPostRedisplay();
        break;
    case 'b': case 'B':
        depth_test_on = 1 - depth_test_on;
        if (depth_test_on) {
            glEnable(GL_DEPTH_TEST);
            fprintf(stdout, "^^^ Depth test enabled.\n");
        }
        else {
            glDisable(GL_DEPTH_TEST);
            fprintf(stdout, "^^^ Depth test disabled.\n");
        }
        glutPostRedisplay();
        break;
    default:
        return;
    }

    {
        glm::vec3 eye = mainCam.cam_view.pos;
        glm::vec3 center = eye - mainCam.cam_view.naxis;
        glm::vec3 up = mainCam.cam_view.vaxis;
        mainCam.ViewMatrix = glm::lookAt(eye, center, up);

        glm::mat3 Rt = glm::transpose(glm::mat3(mainCam.ViewMatrix));
        glm::mat4 T = glm::mat4(Rt) * mainCam.ViewMatrix;
        mainCam.cam_view.uaxis = glm::vec3(mainCam.ViewMatrix[0][0], mainCam.ViewMatrix[1][0], mainCam.ViewMatrix[2][0]);
        mainCam.cam_view.vaxis = glm::vec3(mainCam.ViewMatrix[0][1], mainCam.ViewMatrix[1][1], mainCam.ViewMatrix[2][1]);
        mainCam.cam_view.naxis = glm::vec3(mainCam.ViewMatrix[0][2], mainCam.ViewMatrix[1][2], mainCam.ViewMatrix[2][2]);
        mainCam.cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
    }

    if (dCctv.flag_valid && dCctv.is_dynamic) {
        glm::vec3 eye = dCctv.cam_view.pos;
        glm::vec3 center = eye - dCctv.cam_view.naxis;
        glm::vec3 up = dCctv.cam_view.vaxis;
        dCctv.ViewMatrix = glm::lookAt(eye, center, up);

        glm::mat3 Rt = glm::transpose(glm::mat3(dCctv.ViewMatrix));
        glm::mat4 T = glm::mat4(Rt) * dCctv.ViewMatrix;
        dCctv.cam_view.uaxis = glm::vec3(dCctv.ViewMatrix[0][0], dCctv.ViewMatrix[1][0], dCctv.ViewMatrix[2][0]);
        dCctv.cam_view.vaxis = glm::vec3(dCctv.ViewMatrix[0][1], dCctv.ViewMatrix[1][1], dCctv.ViewMatrix[2][1]);
        dCctv.cam_view.naxis = glm::vec3(dCctv.ViewMatrix[0][2], dCctv.ViewMatrix[1][2], dCctv.ViewMatrix[2][2]);
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    if (activeCameraID == CAMERA_MAIN) {
        Perspective_Camera& mainCam =
            static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_MAIN]].get());
        const float rotateSpeedMain = 1.0f * TO_RADIAN;

        switch (key) {
        case GLUT_KEY_UP:
        {
            glm::vec3 axis = mainCam.cam_view.uaxis;
            float angle = +rotateSpeedMain;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.naxis, 0.0f)));
            mainCam.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.vaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_DOWN:  
        {
            glm::vec3 axis = mainCam.cam_view.uaxis;
            float angle = -rotateSpeedMain;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.naxis, 0.0f)));
            mainCam.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.vaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_LEFT:  
        {
            glm::vec3 axis = mainCam.cam_view.vaxis;
            float angle = +rotateSpeedMain;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.naxis, 0.0f)));
            mainCam.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.uaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_RIGHT:
        {
            glm::vec3 axis = mainCam.cam_view.vaxis;
            float angle = -rotateSpeedMain;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            mainCam.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.naxis, 0.0f)));
            mainCam.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(mainCam.cam_view.uaxis, 0.0f)));
        }
        break;
        default:
            return;
        }

        {
            glm::vec3 eye = mainCam.cam_view.pos;
            glm::vec3 center = eye - mainCam.cam_view.naxis;
            glm::vec3 up = mainCam.cam_view.vaxis;
            mainCam.ViewMatrix = glm::lookAt(eye, center, up);

            glm::mat3 Rt = glm::transpose(glm::mat3(mainCam.ViewMatrix));
            glm::mat4 T = glm::mat4(Rt) * mainCam.ViewMatrix;
            mainCam.cam_view.uaxis = glm::vec3(mainCam.ViewMatrix[0][0], mainCam.ViewMatrix[1][0], mainCam.ViewMatrix[2][0]);
            mainCam.cam_view.vaxis = glm::vec3(mainCam.ViewMatrix[0][1], mainCam.ViewMatrix[1][1], mainCam.ViewMatrix[2][1]);
            mainCam.cam_view.naxis = glm::vec3(mainCam.ViewMatrix[0][2], mainCam.ViewMatrix[1][2], mainCam.ViewMatrix[2][2]);
            mainCam.cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);
        }
    }
    else if (activeCameraID == CAMERA_DYN_CCTV) {
        Perspective_Camera& dCctv =
            static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_DYN_CCTV]].get());
        const float rotateSpeedDyn = 0.5f * TO_RADIAN;

        switch (key) {
        case GLUT_KEY_UP:   
        {
            glm::vec3 axis = dCctv.cam_view.uaxis;
            float angle = +rotateSpeedDyn;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.naxis, 0.0f)));
            dCctv.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.vaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_DOWN: 
        {
            glm::vec3 axis = dCctv.cam_view.uaxis;
            float angle = -rotateSpeedDyn;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.naxis, 0.0f)));
            dCctv.cam_view.vaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.vaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_LEFT:  
        {
            glm::vec3 axis = dCctv.cam_view.vaxis;
            float angle = +rotateSpeedDyn;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.naxis, 0.0f)));
            dCctv.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.uaxis, 0.0f)));
        }
        break;
        case GLUT_KEY_RIGHT: 
        {
            glm::vec3 axis = dCctv.cam_view.vaxis;
            float angle = -rotateSpeedDyn;
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, axis);
            dCctv.cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.naxis, 0.0f)));
            dCctv.cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(dCctv.cam_view.uaxis, 0.0f)));
        }
        break;
        default:
            return;
        }

        {
            glm::vec3 eye = dCctv.cam_view.pos;
            glm::vec3 center = eye - dCctv.cam_view.naxis;
            glm::vec3 up = dCctv.cam_view.vaxis;
            dCctv.ViewMatrix = glm::lookAt(eye, center, up);

            glm::mat3 Rt = glm::transpose(glm::mat3(dCctv.ViewMatrix));
            glm::mat4 T = glm::mat4(Rt) * dCctv.ViewMatrix;
            dCctv.cam_view.uaxis = glm::vec3(dCctv.ViewMatrix[0][0], dCctv.ViewMatrix[1][0], dCctv.ViewMatrix[2][0]);
            dCctv.cam_view.vaxis = glm::vec3(dCctv.ViewMatrix[0][1], dCctv.ViewMatrix[1][1], dCctv.ViewMatrix[2][1]);
            dCctv.cam_view.naxis = glm::vec3(dCctv.ViewMatrix[0][2], dCctv.ViewMatrix[1][2], dCctv.ViewMatrix[2][2]);
        }
    }

    glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (activeCameraID == CAMERA_MAIN) {
        Perspective_Camera& mainCam =
            static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_MAIN]].get());

        float& fovy = mainCam.cam_proj.params.pers.fovy;
        float  aspect = mainCam.cam_proj.params.pers.aspect;
        float  n = mainCam.cam_proj.params.pers.n;
        float  f = mainCam.cam_proj.params.pers.f;
        const float mainZoomSpeed = 1.0f * TO_RADIAN;

        if (direction > 0) {
            fovy -= mainZoomSpeed;
            if (fovy < 1.0f * TO_RADIAN)   fovy = 1.0f * TO_RADIAN;
        }
        else {
            fovy += mainZoomSpeed;
            if (fovy > 90.0f * TO_RADIAN)  fovy = 90.0f * TO_RADIAN;
        }

        mainCam.ProjectionMatrix = glm::perspective(fovy, aspect, n, f);
    }
    else if (activeCameraID == CAMERA_DYN_CCTV) {
        Perspective_Camera& dCctv =
            static_cast<Perspective_Camera&>(scene.camera_list[camera_ID_mapper[CAMERA_DYN_CCTV]].get());

        float& fovy_d = dCctv.cam_proj.params.pers.fovy;
        float  aspect = dCctv.cam_proj.params.pers.aspect;
        float  n = dCctv.cam_proj.params.pers.n;
        float  f = dCctv.cam_proj.params.pers.f;
        float  speed = dCctv.zoomSpeed;

        if (direction > 0) {
            fovy_d -= speed;
            if (fovy_d < 1.0f * TO_RADIAN) fovy_d = 1.0f * TO_RADIAN;
        }
        else {
            fovy_d += speed;
            if (fovy_d > 90.0f * TO_RADIAN) fovy_d = 90.0f * TO_RADIAN;
        }

        dCctv.ProjectionMatrix = glm::perspective(fovy_d, aspect, n, f);
    }

    glutPostRedisplay();
}

void reshape(int width, int height) {
    scene.window.width = width;
    scene.window.height = height;
    scene.window.aspect_ratio = (float)width / height;

    for (auto& refCam : scene.camera_list) {
        Camera& cam = refCam.get();
        if (!cam.flag_valid) continue;

        switch (cam.camera_id) {
        case CAMERA_MAIN: {
            cam.view_port.x = 200;
            cam.view_port.y = 200;
            cam.view_port.w = width - 200;
            cam.view_port.h = height - 200;

            Perspective_Camera& p = static_cast<Perspective_Camera&>(cam);
            p.cam_proj.params.pers.aspect = scene.window.aspect_ratio;
            cam.ProjectionMatrix = glm::perspective(
                p.cam_proj.params.pers.fovy,
                p.cam_proj.params.pers.aspect,
                p.cam_proj.params.pers.n,
                p.cam_proj.params.pers.f
            );
            break;
        }

        case CAMERA_SIDE_FRONT: {
            cam.view_port.x = 0;
            cam.view_port.y = 8 * (height / 10);
            cam.view_port.w = width / 4;
            cam.view_port.h = height / 4;
            break;
        }

        case CAMERA_TOP: {
            cam.view_port.x = 0;
            cam.view_port.y = 5.5 * (height / 10);
            cam.view_port.w = width / 4;
            cam.view_port.h = height / 4;
            break;
        }

        case CAMERA_SIDE: {
            cam.view_port.x = 0;
            cam.view_port.y = 3 * (height / 10);
            cam.view_port.w = width / 4;
            cam.view_port.h = height / 4;
            break;
        }

        case CAMERA_CC_0: {
            cam.view_port.x = width - 200;
            cam.view_port.y = height * 7 / 10;
            cam.view_port.w = 200;
            cam.view_port.h = 200;
            break;
        }

        case CAMERA_CC_1: {
            cam.view_port.x = width - 200;
            cam.view_port.y = height * 4 / 10;
            cam.view_port.w = 200;
            cam.view_port.h = 200;
            break;
        }

        case CAMERA_CC_2: {
            cam.view_port.x = width - 200;
            cam.view_port.y = height * 1 / 10;
            cam.view_port.w = 200;
            cam.view_port.h = 200;
            break;
        }

        case CAMERA_DYN_CCTV: {
            cam.view_port.x = 0;
            cam.view_port.y = 7 * (height / 10);
            cam.view_port.w = width / 4;
            cam.view_port.h = height / 4;

            Perspective_Camera& pd = static_cast<Perspective_Camera&>(cam);
            pd.cam_proj.params.pers.aspect = scene.window.aspect_ratio;
            cam.ProjectionMatrix = glm::perspective(
                pd.cam_proj.params.pers.fovy,
                pd.cam_proj.params.pers.aspect,
                pd.cam_proj.params.pers.n,
                pd.cam_proj.params.pers.f
            );
            break;
        }

        case CAMERA_TIGER_EYE:
            cam.view_port.x = 0;
            cam.view_port.y = 0;
            cam.view_port.w = width / 4; 
            cam.view_port.h = height / 4;
            {
                Perspective_Camera& p = static_cast<Perspective_Camera&>(cam);
                p.cam_proj.params.pers.aspect = scene.window.aspect_ratio;
                cam.ProjectionMatrix = glm::perspective(
                    p.cam_proj.params.pers.fovy,
                    p.cam_proj.params.pers.aspect,
                    p.cam_proj.params.pers.n,
                    p.cam_proj.params.pers.f
                );
            }
            break;

        default:
            break;
        }
    }

    glutPostRedisplay();
}

void timer_scene(int index) {
    if (glutGetWindow() == 0) return;

    scene.clock(0);
    glutPostRedisplay();
    glutTimerFunc(100, timer_scene, 0);
}

void register_callbacks(void) {
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMouseWheelFunc(mouseWheel);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, timer_scene, 0);
    //	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
    glEnable(GL_DEPTH_TEST); // Default state

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
}

void initialize_renderer(void) {
    register_callbacks();
    initialize_OpenGL();
    scene.initialize();

    scene.create_camera_list(
        scene.window.width,
        scene.window.height,
        scene.window.aspect_ratio
    );
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

void print_message(const char* m) {
    fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
    fprintf(stdout, "**************************************************************\n\n");
    fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
    fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
    fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

    for (int i = 0; i < n_message_lines; i++)
        fprintf(stdout, "%s\n", messages[i]);
    fprintf(stdout, "\n**************************************************************\n\n");

    initialize_glew();
}

#define N_MESSAGE_LINES 10
void main(int argc, char* argv[]) {
    char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
    char messages[N_MESSAGE_LINES][256] = {
        "  Keys used:",
        "    W/S/A/D/Q/E : 이동 (상/하/좌/우/전/후)",
        "    Z/X         : Roll 회전",
        "    Arrow Keys  : Pitch/Yaw 회전",
        "    Mouse Wheel : Zoom In/Out",
        "    C/V/B       : Culling / Polygon 모드 / Depth Test 토글",
        "    F           : 카메라 그룹 전환",
        "    T           : 카메라 프레임(RGB 좌표축) 생성/해제",
        "    1/2         : 주 카메라 작동 / 동적 카메라 작동",
        "    3           : 호랑이 눈 카메라 화면 토글"
    };

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1200, 800);
    glutInitContextVersion(4, 0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow(program_name);

    greetings(program_name, messages, N_MESSAGE_LINES);
    initialize_renderer();

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutMainLoop();
}
