#ifndef UNIFORMRIGIDBODY_H
#define UNIFORMRIGIDBODY_H

#include <vector>
#include "VectorMath.h"

struct UniformRigidBody
{

    //constants
    float mass;
    float inertia;
    float massInv;
    float inertiaInv;
    float elasticity = 0.25f;

    //state variables
    Vector3 position;
    Quaternion rotation;
    Matrix3 rotationMatrix;
    Vector3 linearMomentum;
    Vector3 angularMomentum;

    //derived quantities
    Vector3 velocity;
    Vector3 angularVelocity;

    //known quantities
    Vector3 gravitionalForce;
    Vector3 torque;

    //applied force/torque will be applie for a single step
    std::vector<Vector3> appliedForces;
    std::vector<Vector3> appliedTorques;
    bool applyForce = false;
    bool applyTorque = false;
    bool dynamic = true;
    bool applyGravity = true;
    bool atRest = false;
    bool restingContact = false;
    bool stabilizing = false;


    UniformRigidBody(float _mass, float _inertia);
    UniformRigidBody();
    UniformRigidBody(const UniformRigidBody& other);
    UniformRigidBody& operator= (const UniformRigidBody& other);
    virtual ~UniformRigidBody();
    void addForce(const Vector3& force);
    void addTorque(const Vector3& torque);
    void addForce(const Vector3& force,  UniformRigidBody& other);
    void addTorque(const Vector3& torque,  UniformRigidBody& other);
    void setVelocity(const Vector3& velocity);
    void setAngularVelocity(const Vector3& angularVelocity);
    Vector3 getLocalXAxis();
    Vector3 getLocalYAxis();
    Vector3 getLocalZAxis();
    Vector3 peekNextPosition(float dt);
    void stepQuantities(float dt);
    bool isStatic();
};
#endif // UNIFORMRIGIDBODY_H
