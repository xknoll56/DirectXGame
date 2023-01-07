#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <vector>
#include "Collider.h"
struct RayCastData
{
    Vector3 point;
    Vector3 normal;
    float length;
    Collider* collider;
};

struct ContactInfo
{
    float penetrationDistance;
    Vector3 normal;
    std::vector<Vector3> points;
    BoxCollider::ContactDir aDir;
    BoxCollider::ContactDir bDir;
    int vertexPoints;
    int edgePoints;
    Collider* a;
    Collider* b;
    bool faceCollision;
    bool faceToFaceCollision;

    ContactInfo()
    {
        penetrationDistance = -std::numeric_limits<float>().max();
        normal = Vector3();
        aDir = BoxCollider::ContactDir::NONE;
        bDir = BoxCollider::ContactDir::NONE;
        faceCollision = true;
        faceToFaceCollision = false;
        vertexPoints = 0;
        edgePoints = 0;
    }
};

struct PhysicsWorld
{
    Vector3 gravity;
    std::vector<Collider*> colliders;
    RayCastData rcd;
    float friction = 0.4f;
    float restitutionSlope = 0.085f;
    float restitutionIntersect = 0.4f;
    std::vector<ContactInfo> contacts;
    bool enableResponse = true;

    PhysicsWorld(std::vector<Collider*>* colliders, Vector3 gravity);
    PhysicsWorld(std::vector<Collider*>* colliders);
    PhysicsWorld();
    void setColliders(std::vector<Collider*>* colliders);
    bool contactHandled(Collider* a, Collider* b);
    void checkForCollisions(float dt);
    void CollisionResponse(float dt);
    Vector3 closestPointBetweenLines(Vector3& p0,  Vector3& p1, const Vector3& u, const Vector3& v);
    float closestDistanceBetweenLines(Vector3& p0,  Vector3& p1, const Vector3& u, const Vector3& v, float s0, float s1);
    bool closestPointsDoIntersect(Vector3& p0,  Vector3& p1,  const Vector3& u, const Vector3& v, float s0, float s1, float max1, float max2);
    bool detectCubeCubeCollision(float dt, BoxCollider* cubeA, BoxCollider* cubeB, ContactInfo& contactInfo);
    void determineCubeCubeContactPoints(ContactInfo& info, BoxCollider* cubeA, BoxCollider* cubeB);
    bool detectCubeSphereCollision(float dt, BoxCollider* cube, SphereCollider* sphere, ContactInfo& contactInfo);
    void determineCubeCubePetrusionVerts(ContactInfo& info,const Vector3& normal, const std::vector<Vector3>& points, BoxCollider* toCube, BoxCollider::ContactDir dir, bool adjustPenetration);
    bool isCubeCubePetrusion(const Vector3& normal, const std::vector<Vector3>& points, BoxCollider* toCube, BoxCollider::ContactDir dir);
    void cubeCubeCollisionResponse(ContactInfo& info, float dt, BoxCollider* cubeA, BoxCollider* cubeB);
    void cubeCubeCollisionResponseDynamicVsStatic(ContactInfo& info, const Vector3& norm, float dt, BoxCollider* dynamicCube, BoxCollider* staticCube);
    void cubeSphereCollisionResponseStaticVsDynamic(ContactInfo& info, float dt, BoxCollider* cube, SphereCollider* sphere);
    void cubeSphereCollisionResponseDynamicVsDynamic(ContactInfo& info, float dt, BoxCollider* cube, SphereCollider* sphere);
    bool detectSphereSphereCollision(SphereCollider* sphere, SphereCollider* other);
    void sphereSphereCollisionResponse(float dt, SphereCollider* sphere, SphereCollider* other);
    void updateQuantities(float dt);
    void stepWorld(float dt);
    void stepWorld(float dt, int inc);
    bool cubeFlatOnSurface(BoxCollider* cube, Vector3& normal, float tolerance);
    bool cubeRaycast(const Vector3& start, const Vector3& dir, RayCastData& dat, BoxCollider* cube);
    bool sphereRaycast(const Vector3& start, const Vector3& dir, RayCastData& dat, SphereCollider* sphere);
    bool raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat);
    bool raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat, int mask);
    bool raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat, ColliderType type);
    bool narrowPhaseCollisionDetection(Collider& colA, Collider& colB);
};



#endif // PHYSICSWORLD_H
