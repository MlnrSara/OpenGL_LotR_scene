#include "Camera.hpp"
#include <glm/gtx/euler_angles.hpp>
namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->sceneUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->sceneUpDirection));
        this->cameraRightDirection *= -1;
        this->cameraUpDirection = (glm::cross(-this->cameraFrontDirection, this->cameraRightDirection));
        this->planeBind = true;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        glm::mat4 view;
        view = glm::lookAt(this->cameraPosition, this->cameraTarget, this->sceneUpDirection);
        return view;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        if (this->planeBind) {
            if (direction == MOVE_BACKWARD) {
                this->cameraPosition += speed * (-1) * (this->cameraFrontDirection);
            }
            if (direction == MOVE_FORWARD) {
                this->cameraPosition += speed * (this->cameraFrontDirection);
            }
            if (direction == MOVE_RIGHT) {
                this->cameraPosition += speed * this->cameraRightDirection;
            }
            if (direction == MOVE_LEFT) {
                this->cameraPosition += speed * (-1) * this->cameraRightDirection;
            }
            this->cameraPosition.y = 0.75f;
        }
        else {
            if (direction == MOVE_BACKWARD) {
                this->cameraPosition += speed * (-1) * (this->cameraFrontDirection);
            }
            if (direction == MOVE_FORWARD) {
                this->cameraPosition += speed * (this->cameraFrontDirection);
            }
            if (direction == MOVE_LEFT) {
                this->cameraPosition += speed * this->cameraRightDirection;
            }
            if (direction == MOVE_RIGHT) {
                this->cameraPosition += speed * (-1) * this->cameraRightDirection;
            }
        }

        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::mat4 rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
        this->cameraFrontDirection = glm::normalize(rotation * glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0.0f));
        this->cameraRightDirection = -glm::normalize(glm::cross(-this->cameraFrontDirection, this->sceneUpDirection));
        this->cameraUpDirection = glm::cross(-this->cameraFrontDirection, this->cameraRightDirection);
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;
    }

    bool Camera::getPlaneBindVar() {
        return this->planeBind;
    }

    void Camera::setPlaneBindVar(bool planeBind) {
        this->planeBind = planeBind;
    }
}