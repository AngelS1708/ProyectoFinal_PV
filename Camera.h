#ifndef CAMERA_H
#define CAMERA_H

#include "reactphysics3d/reactphysics3d.h"
using namespace rp3d;

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    JUMP
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool isCameraDebugger;
    bool isJump;
    //bool isInFloor;

    float jumpForce;

    Camera(vec3 position = vec3(0.0f,0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        isCameraDebugger = false;
        isJump = false;
        //isInFloor = true;

        
        jumpForce = 14.0f;

        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = vec3(posX, posY, posZ);
        WorldUp = vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    mat4 GetViewMatrix()
    {
        return lookAt(Position, Position + Front, Up);
    }

    void ProcessBodyMovement(Camera_Movement direction, float deltaTime, GLFWwindow* window, PhysicsWorld* world, RigidBody* body )
    {


        ProcessKeyboard(direction, deltaTime);

    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        vec3 horizontalFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));

        if (isCameraDebugger){
            if (direction == FORWARD)
                Position += horizontalFront * velocity;
            if (direction == BACKWARD)
                Position -= horizontalFront * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
            if (direction == UP)
                Position.y += Up.y * velocity;
            if (direction == DOWN)
                Position.y -= Up.y * velocity;
        }

        /*
        if (direction == JUMP && !isCameraDebugger && !isJump)
        {
            isJump = true;
        }
        if (!isCameraDebugger && !isJump && isInFloor)
        {
            Position.y = 10.0f;
        }
        */
    }

    void updateCamera(GLFWwindow *window ,RigidBody *body, PhysicsWorld *world, float deltaTime, bool isInFloor) {

        if (!isCameraDebugger)
        {
            Transform currentTransform = body->getTransform();
            bool isJump = false;
            //float jumpForce = 13.0f;

            // Quaternion bodyOrientation = currentTransform.getOrientation();


            Vector3 displacement(0.0f, 0.0f, 0.0f);

            Quaternion newOrientation = Quaternion::identity();
            Vector3 angle(0.0f, -radians(this->Yaw), 0.0f);
            newOrientation = Quaternion::fromEulerAngles(angle);


            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                displacement += newOrientation * Vector3(this->MovementSpeed, 0.0f, 0.0f);

            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                displacement += newOrientation * Vector3(-this->MovementSpeed, 0.0f, 0.0f);

            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                displacement += newOrientation * Vector3(0.0f, 0.0f, -this->MovementSpeed);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                displacement += newOrientation * Vector3(0.0f, 0.0f, this->MovementSpeed);
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS  && this->isJump == false)
            {
                this->isJump = true;
                    //displacement += newOrientation * Vector3(0.0f, jumpForce, 0.0f);
            }

            if (this->isJump)
            {
                jumpForce -= 9.8f * deltaTime;
                displacement += newOrientation * Vector3(0.0f, jumpForce, 0.0f);

            }

            if (isInFloor == true)
            {
                this->isJump = false;
                jumpForce = 14.0f;
            }



            Vector3 newPosition = currentTransform.getPosition() + displacement * deltaTime;
            Transform newTransform(newPosition, newOrientation);

            //newTransform = Transform::identity();

            body->setTransform(newTransform);
            world->update(deltaTime);

            Position = vec3(body->getTransform().getPosition().x, body->getTransform().getPosition().y + 1.25, body->getTransform().getPosition().z);
        }

    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    void updateCameraVectors()
    {
        vec3 front;
        front.x = cos(radians(Yaw)) * cos(radians(Pitch));
        front.y = sin(radians(Pitch));
        front.z = sin(radians(Yaw)) * cos(radians(Pitch));
        Front = normalize(front); 

        Right = normalize(cross(Front, WorldUp)); 
        Up = normalize(cross(Right, Front));
    }
};
#endif
