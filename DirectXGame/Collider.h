#ifndef COLLIDER_H
#define COLLIDER_H

#include "UniformRigidBody.h"

enum ColliderType
{
    SPHERE = 0,
    CUBE = 1,
    PLANE = 2,
    QUAD = 3,
    NONE = 4
};

struct AABB
{
    float xSize;
    float ySize;
    float zSize;
};


struct Collider
{
    ColliderType type;
    UniformRigidBody* rb = nullptr;
    bool collisionDetected = false;
    unsigned int id;
    int mask;
    AABB aabb;
    Collider()
    {
        type = ColliderType::NONE;
    }
    Collider(const Collider& other)
    {
        type = other.type;
        rb = other.rb;
        collisionDetected = other.collisionDetected;
        id = other.id;
        aabb = other.aabb;
        mask = other.mask;
    }
    Collider& operator= (const Collider& other)
    {
        type = other.type;
        rb = other.rb;
        collisionDetected = other.collisionDetected;
        id = other.id;
        aabb = other.aabb;
        mask = other.mask;
        return *this;
    }
    virtual ~Collider();
};

struct PlaneCollider: public Collider
{
    Vector3 normal;
    Vector3 point1, point2, point3;
    PlaneCollider(const Vector3& point1, const Vector3& point2, const Vector3& point3);
};


struct SphereCollider: public Collider
{
    float radius;
    Vector3 scale;

    SphereCollider();
    SphereCollider(const float radius);
    SphereCollider(const float radius, UniformRigidBody* const rb);
    SphereCollider& operator= (const SphereCollider& other);
    SphereCollider(const SphereCollider& other);
    void setAABB();
};

struct BoxCollider: public Collider
{
    //The sizes from the origin of the cube (halfs)
    float xSize, ySize, zSize;
    Vector3 scale;
    Vector3 contactVertBuffer[8];
    enum ContactDir
    {
        RIGHT = 0,
        UP = 1,
        FORWARD = 2,
        NONE = 3
    };
    struct EdgeIndices
    {
        int i0;
        int i1;
        float length;
        //the midpoints will be updated every request for edges
        Vector3 midPoint;
        Vector3 dir;
        //for the collision detection the length of the other edge normal to the edges must be saved
        float normalLength;
    };

    EdgeIndices edges[12];
    std::vector<int> indices;



    BoxCollider();
    BoxCollider(const BoxCollider& other);
    BoxCollider(const Vector3& sizes);
    BoxCollider& operator= (const BoxCollider& other);
    void initEdges();
    void updateContactVerts();
    void updateContactEdges();
    std::vector<EdgeIndices> getEdgesFromVertexIndices();
    std::vector<Vector3> getClosestVerts(const Vector3& dir);
    std::vector<EdgeIndices> getClosestEdges(const Vector3& dir);
    ContactDir GetFaceClosestToNormal(const Vector3& normal, bool& negative, Vector3& faceDir);
    Quaternion toRotation(ContactDir upMostDir, const Vector3& up);
    void setAABB();

};

struct QuadCollider: public Collider
{
    float xSize, zSize;

    QuadCollider(const float xSize, const float zSize);
};
#endif // COLLIDER_H
