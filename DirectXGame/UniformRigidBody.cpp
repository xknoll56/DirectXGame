#include "UniformRigidBody.h"
#include <algorithm>


UniformRigidBody::UniformRigidBody(float _mass, float _inertia)
{
    mass = _mass;
    inertia = _inertia;
    massInv = 1.0f/mass;
    inertiaInv = 1.0f/inertia;
    position = Vector3();
    rotation = Quaternion::FromEulerAngles(Vector3(0,0,0));
    linearMomentum = Vector3();
    angularMomentum = Vector3();
    velocity = Vector3();
    angularVelocity = Vector3();
    gravitionalForce = Vector3();
    torque = Vector3();
    rotation.normalize();
    rotationMatrix = rotation.ToMatrix3();
}

UniformRigidBody::UniformRigidBody(const UniformRigidBody& other)
{
    mass = other.mass;
    inertia = other.inertia;
    massInv = 1.0f/other.mass;
    inertiaInv = 1.0f/other.inertia;
    position = other.position;
    rotation = other.rotation;
    linearMomentum = other.linearMomentum;
    angularMomentum = other.angularMomentum;
    velocity = other.velocity;
    angularVelocity = other.angularVelocity;
    gravitionalForce = other.gravitionalForce;
    torque = other.torque;
    rotation.normalize();
    rotationMatrix = rotation.ToMatrix3();
}

UniformRigidBody& UniformRigidBody::operator= (const UniformRigidBody& other)
{
    mass = other.mass;
    inertia = other.inertia;
    massInv = 1.0f/other.mass;
    inertiaInv = 1.0f/other.inertia;
    position = other.position;
    rotation = other.rotation;
    linearMomentum = other.linearMomentum;
    angularMomentum = other.angularMomentum;
    velocity = other.velocity;
    angularVelocity = other.angularVelocity;
    gravitionalForce = other.gravitionalForce;
    torque = other.torque;
    rotation.normalize();
    rotationMatrix = rotation.ToMatrix3();
    return *this;
}

UniformRigidBody::UniformRigidBody(): mass(1.0f), inertia(1.0f)
{
    massInv = 1.0f/mass;
    inertiaInv = 1.0f/inertia;
    position = Vector3();
    rotation = Quaternion::FromEulerAngles(Vector3(0,0,0));
    linearMomentum = Vector3();
    angularMomentum = Vector3();
    velocity = Vector3();
    angularVelocity = Vector3();
    gravitionalForce = Vector3();
    torque = Vector3();
    rotation.normalize();
    rotationMatrix = rotation.ToMatrix3();
}


UniformRigidBody::~UniformRigidBody()
{

}

void UniformRigidBody::addForce(const Vector3& force)
{
    appliedForces.push_back(force);
    applyForce = true;
}

void UniformRigidBody::addTorque(const Vector3& torque)
{
    appliedTorques.push_back(torque);
    applyTorque = true;
}

void UniformRigidBody::addForce(const Vector3& force, UniformRigidBody& other)
{
    appliedForces.push_back(force);
    applyForce = true;
}
void UniformRigidBody::addTorque(const Vector3& torque,  UniformRigidBody& other)
{
    appliedTorques.push_back(torque);
    applyTorque = true;
}

void UniformRigidBody::setVelocity(const Vector3& velocity)
{
    linearMomentum = velocity*mass;
    this->velocity = velocity;
}

void UniformRigidBody::setAngularVelocity(const Vector3& angularVelocity)
{
    angularMomentum = angularVelocity*inertia;
    this->angularVelocity = angularVelocity;
}

Vector3 UniformRigidBody::getLocalXAxis()
{
    return Vector3(rotationMatrix.mat[0]);
}

Vector3 UniformRigidBody::getLocalYAxis()
{
    return Vector3(rotationMatrix.mat[1]);
}

Vector3 UniformRigidBody::getLocalZAxis()
{
    return Vector3(rotationMatrix.mat[2]);
}

Vector3 UniformRigidBody::peekNextPosition(float dt)
{
    Vector3 temp = gravitionalForce * dt;
    Vector3 tempMomentum = linearMomentum+temp;
    temp = massInv * tempMomentum * dt;
    return position + temp;
}
void UniformRigidBody::stepQuantities(float dt)
{

    if(dynamic && !atRest)
    {
        if(applyForce)
        {
            for(Vector3 force: appliedForces)
                linearMomentum+=dt*force;
            applyForce = false;
            appliedForces.clear();
        }
        if(applyTorque)
        {
            for(Vector3 torque: appliedTorques)
                angularMomentum+=dt*torque;
            applyTorque = false;
            appliedTorques.clear();
        }
        angularMomentum += torque*dt;
        if(applyGravity)
            linearMomentum += gravitionalForce*dt;
        angularVelocity = inertiaInv*angularMomentum;
        velocity = massInv*linearMomentum;
        Quaternion angularVelQuat(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        rotation = rotation + (dt*0.5f) * angularVelQuat *rotation;
        rotation.normalize();
        rotationMatrix = rotation.ToMatrix3();
        position+=dt*velocity;
    }
    else
    {
        angularMomentum = Vector3();
        linearMomentum = Vector3();
        rotation.normalize();
        rotationMatrix = rotation.ToMatrix3();
    }
}

bool UniformRigidBody::isStatic()
{
    return !dynamic || atRest;
}
