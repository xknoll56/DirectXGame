#include "PhysicsWorld.h"


PhysicsWorld::PhysicsWorld(std::vector<Collider*>* colliders, Vector3 gravity)
{
    this->gravity = gravity;
    this->colliders.reserve(colliders->size());
    unsigned int i = 0;
    for (auto& collider : *colliders)
    {
        collider->id = i++;
        this->colliders.push_back(collider);
    }
    for (auto& collider : *colliders)
    {
        if (collider->rb != nullptr)
            collider->rb->gravitionalForce += collider->rb->mass * gravity;
    }

}

PhysicsWorld::PhysicsWorld(std::vector<Collider*>* colliders)
{
    gravity = Vector3(0, -9.81f, 0);
    this->colliders.reserve(colliders->size());
    unsigned int i = 0;
    for (auto& collider : *colliders)
    {
        collider->id = i++;
        this->colliders.push_back(collider);
    }
    for (auto& collider : *colliders)
    {
        if (collider->rb != nullptr)
            collider->rb->gravitionalForce += collider->rb->mass * gravity;
    }

}

PhysicsWorld::PhysicsWorld()
{
    gravity = Vector3(0, -9.81f, 0);
}

void PhysicsWorld::setColliders(std::vector<Collider*>* colliders)
{
    this->colliders.reserve(colliders->size());
    unsigned int i = 0;
    for (auto& collider : *colliders)
    {
        collider->id = i++;
        this->colliders.push_back(collider);
    }
    for (auto& collider : *colliders)
    {
        if (collider->rb != nullptr)
            collider->rb->gravitionalForce += collider->rb->mass * gravity;
    }
}

bool PhysicsWorld::contactHandled(Collider* a, Collider* b)
{
    for (ContactInfo& contact : contacts)
    {
        if ((contact.a->id == a->id && contact.b->id == b->id) || (contact.b->id == a->id && contact.a->id == b->id))
            return true;
    }

    return false;
}


void PhysicsWorld::checkForCollisions(float dt)
{
    contacts.clear();
    for (auto& collider : colliders)
    {
        switch (collider->type)
        {
        case ColliderType::SPHERE:
        {
            SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider);
            //spherePlaneCollision(dt, sphere);
            for (auto& other : colliders)
            {
                if (other != collider)
                {
                    switch (other->type)
                    {
                    case ColliderType::SPHERE:
                    {
                        SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other);
                        if (detectSphereSphereCollision(sphere, otherSphere))
                            sphereSphereCollisionResponse(dt, sphere, otherSphere);
                        break;
                    }
                    case ColliderType::CUBE:
                    {
                        BoxCollider* otherCube = dynamic_cast<BoxCollider*>(other);
                        ContactInfo info;
                        if (!contactHandled(otherCube, sphere))
                        {
                            if (detectCubeSphereCollision(dt, otherCube, sphere, info))
                            {

                            }
                        }
                        break;
                    }
                    }
                }
            }
            break;
        }
        case ColliderType::CUBE:
        {
            BoxCollider* cube = dynamic_cast<BoxCollider*>(collider);
            for (auto& other : colliders)
            {
                if (other != collider)
                {
                    switch (other->type)
                    {
                    case ColliderType::CUBE:
                    {
                        BoxCollider* otherCube = dynamic_cast<BoxCollider*>(other);
                        if (!contactHandled(cube, otherCube))
                        {
                            ContactInfo info;
                            if (detectCubeCubeCollision(dt, cube, otherCube, info))
                                determineCubeCubeContactPoints(info, cube, otherCube);
                        }
                        break;
                    }
                    case ColliderType::SPHERE:
                    {
                        SphereCollider* sphere = dynamic_cast<SphereCollider*>(other);
                        ContactInfo info;
                        if (!contactHandled(cube, sphere))
                        {
                            if (detectCubeSphereCollision(dt, cube, sphere, info))
                            {
                                //qDebug() << "collision";
                            }
                        }
                        break;
                    }
                    }
                }
            }
            break;
        }
        }
    }

    CollisionResponse(dt);
}

void PhysicsWorld::CollisionResponse(float dt)
{
    if (enableResponse)
    {
        for (ContactInfo& info : contacts)
        {
            if (info.a->type == ColliderType::CUBE && info.b->type == ColliderType::CUBE)
            {
                BoxCollider* cube = dynamic_cast<BoxCollider*>(info.a);
                BoxCollider* otherCube = dynamic_cast<BoxCollider*>(info.b);
                if (cube && otherCube)
                {
                    if (cube->rb->isStatic() && !otherCube->rb->isStatic())
                    {
                        cubeCubeCollisionResponseDynamicVsStatic(info, -info.normal, dt, otherCube, cube);
                    }
                    else if (otherCube->rb->isStatic() && !cube->rb->isStatic())
                    {
                        cubeCubeCollisionResponseDynamicVsStatic(info, info.normal, dt, cube, otherCube);
                    }
                    else if (!cube->rb->isStatic() && !otherCube->rb->isStatic())
                        cubeCubeCollisionResponse(info, dt, cube, otherCube);
                }
            }
            else if (info.a->type == ColliderType::CUBE && info.b->type == ColliderType::SPHERE)
            {
                BoxCollider* cube = dynamic_cast<BoxCollider*>(info.a);
                SphereCollider* sphere = dynamic_cast<SphereCollider*>(info.b);
                if (cube && sphere)
                {
                    if (cube->rb->isStatic())
                    {
                        cubeSphereCollisionResponseStaticVsDynamic(info, dt, cube, sphere);
                    }
                    else
                        cubeSphereCollisionResponseDynamicVsDynamic(info, dt, cube, sphere);
                }
            }
        }
    }
}

Vector3 PhysicsWorld::closestPointBetweenLines(Vector3& p0, Vector3& p1, const Vector3& u, const Vector3& v)
{
    Vector3 uv = VectorCross(v, u);
    float uvSquared = VectorLengthSquared(uv);
    float t = -VectorDot(VectorCross(p1 - p0, u), uv) / uvSquared;
    float s = -VectorDot(VectorCross(p1 - p0, v), uv) / uvSquared;
    return 0.5f * (p0 + s * u + p1 + t * v);
}

float PhysicsWorld::closestDistanceBetweenLines(Vector3& p0, Vector3& p1, const Vector3& u, const Vector3& v, float s0, float s1)
{
    Vector3 uv = VectorCross(v, u);
    float uvSquared = VectorLengthSquared(uv);
    float t = -VectorDot(VectorCross(p1 - p0, u), uv) / uvSquared;
    float s = -VectorDot(VectorCross(p1 - p0, v), uv) / uvSquared;
    if (fabsf(t) > s1 || fabsf(s) > s0 || EpsilonEqual(t, 0.0f, 0.01f) || EpsilonEqual(s, 0.0f, 0.01f))
        return std::numeric_limits<float>::max();

    return fabsf(VectorDot(uv, p0 - p1) / VectorLength(uv));
}

bool PhysicsWorld::closestPointsDoIntersect(Vector3& p0, Vector3& p1, const Vector3& u, const Vector3& v, float s0, float s1, float max1, float max2)
{
    Vector3 uv = VectorCross(v, u);
    float uvSquared = VectorLengthSquared(uv);
    float t = -VectorDot(VectorCross(p1 - p0, u), uv) / uvSquared;
    float s = -VectorDot(VectorCross(p1 - p0, v), uv) / uvSquared;
    float dist = fabsf(VectorDot(uv, p0 - p1) / VectorLength(uv));
    if (fabsf(t) > s1 || fabsf(s) > s0 || std::isnan(t) || std::isnan(s))
        return false;

    return true;
}


bool PhysicsWorld::detectCubeSphereCollision(float dt, BoxCollider* cube, SphereCollider* sphere, ContactInfo& contactInfo)
{
    cube->collisionDetected = false;
    sphere->collisionDetected = false;

    contactInfo.a = cube;
    contactInfo.b = sphere;
    contactInfo.faceCollision = false;

    Vector3 aX = cube->rb->getLocalXAxis();
    Vector3 aY = cube->rb->getLocalYAxis();
    Vector3 aZ = cube->rb->getLocalZAxis();

    Vector3 T = cube->rb->position - sphere->rb->position;

    float penetrationY, penetrationX, penetrationZ;

    float penetration = fabsf(VectorDot(aX, T)) - cube->xSize - sphere->radius;
    if (penetration > 0)
    {

        return false;
    }
    contactInfo.aDir = BoxCollider::ContactDir::RIGHT;
    contactInfo.penetrationDistance = penetration;
    penetrationX = fabsf(penetration);

    penetration = fabsf(VectorDot(aY, T)) - cube->ySize - sphere->radius;
    if (penetration > 0)
    {
        return false;
    }

    penetrationY = fabsf(penetration);
    if (penetration > contactInfo.penetrationDistance)
    {
        contactInfo.aDir = BoxCollider::ContactDir::UP;
        contactInfo.penetrationDistance = penetration;
    }



    penetration = fabsf(VectorDot(aZ, T)) - cube->zSize - sphere->radius;
    if (penetration > 0)
    {
        return false;
    }
    penetrationZ = penetration;
    if (penetration > contactInfo.penetrationDistance)
    {
        contactInfo.aDir = BoxCollider::ContactDir::FORWARD;
        contactInfo.penetrationDistance = penetration;
    }


    if (cube->rb->atRest)
        cube->rb->atRest = false;

    Vector3 normal, normalX, normalY, normalZ;
    normalX = Sign(VectorDot(aX, T)) * aX;
    normalY = Sign(VectorDot(aY, T)) * aY;
    normalZ = Sign(VectorDot(aZ, T)) * aZ;
    float size;
    Vector3 castDir;
    switch (contactInfo.aDir)
    {
    case(BoxCollider::ContactDir::RIGHT):
        normal = normalX;
        size = cube->xSize;
        castDir = VectorNormalize(-normalY - normalZ);
        if (cubeRaycast(sphere->rb->position, normalX, rcd, cube))
        {
            if (rcd.length < sphere->radius)
            {
                contactInfo.points.push_back(rcd.point);
                contactInfo.normal = -normalX;
                contactInfo.faceCollision = true;
                contacts.push_back(contactInfo);
                return true;
            }
        }
        break;
    case(BoxCollider::ContactDir::UP):
        normal = normalY;
        size = cube->ySize;
        castDir = VectorNormalize(-normalX - normalZ);
        if (cubeRaycast(sphere->rb->position, normalY, rcd, cube))
        {
            if (rcd.length < sphere->radius)
            {
                contactInfo.points.push_back(rcd.point);
                contactInfo.normal = -normalY;
                contactInfo.faceCollision = true;
                contacts.push_back(contactInfo);
                return true;
            }
        }
        break;
    case(BoxCollider::ContactDir::FORWARD):
        if (cubeRaycast(sphere->rb->position, normalZ, rcd, cube))
        {
            if (rcd.length < sphere->radius)
            {
                contactInfo.points.push_back(rcd.point);
                contactInfo.normal = -normalZ;
                contactInfo.faceCollision = true;
                contacts.push_back(contactInfo);
                return true;
            }
        }
        break;
    }

    //Thats not all, need to do the edges...
    std::vector<Vector3> closestVerts = cube->getClosestVerts(T);
    std::vector<BoxCollider::EdgeIndices> edges = cube->getEdgesFromVertexIndices();

    RayCastData rcd2;
    for (BoxCollider::EdgeIndices edge : edges)
    {
        Vector3 point1 = edge.midPoint - edge.dir * edge.length;
        Vector3 dir1 = edge.dir;

        Vector3 point2 = edge.midPoint + edge.dir * edge.length;
        Vector3 dir2 = -edge.dir;
        if (sphereRaycast(point1, dir1, rcd, sphere) && sphereRaycast(point2, dir2, rcd2, sphere))
        {
            Vector3 contactPoint = (rcd.point + rcd2.point) * 0.5f;
            contactInfo.points.push_back(contactPoint);
            contactInfo.normal = VectorNormalize(sphere->rb->position - contactPoint);
            contactInfo.penetrationDistance = VectorLength(contactPoint - sphere->rb->position) - sphere->radius;
            contactInfo.faceCollision = false;
            contacts.push_back(contactInfo);
            return true;
        }
    }


    if (fabsf(VectorDot(aX, T)) < cube->xSize && fabsf(VectorDot(aZ, T)) < cube->zSize && fabsf(VectorDot(aY, T)) < cube->ySize)
    {
        contactInfo.points.push_back(sphere->rb->position);
        contactInfo.normal = -normal;
        contactInfo.faceCollision = true;
        contacts.push_back(contactInfo);
        return true;
    }



    //finally it could be one of the corners.
    for (Vector3 point : closestVerts)
    {

        float dist = VectorLength(sphere->rb->position - point);
        if (dist < sphere->radius)
        {
            contactInfo.points.push_back(point);
            contactInfo.normal = VectorNormalize(sphere->rb->position - point);
            contactInfo.penetrationDistance = dist - sphere->radius;
            contactInfo.faceCollision = false;
            contacts.push_back(contactInfo);
            return true;
        }

    }


    return false;

}


bool PhysicsWorld::detectCubeCubeCollision(float dt, BoxCollider* cubeA, BoxCollider* cubeB, ContactInfo& contactInfo)
{
    if (!cubeA->rb->dynamic && !cubeB->rb->dynamic)
        return false;

    Vector3 aX = cubeA->rb->getLocalXAxis();
    Vector3 aY = cubeA->rb->getLocalYAxis();
    Vector3 aZ = cubeA->rb->getLocalZAxis();

    Vector3 bX = cubeB->rb->getLocalXAxis();
    Vector3 bY = cubeB->rb->getLocalYAxis();
    Vector3 bZ = cubeB->rb->getLocalZAxis();

    Vector3 T = cubeB->rb->position - cubeA->rb->position;

    float rxx = VectorDot(aX, bX);
    float rxy = VectorDot(aX, bY);
    float rxz = VectorDot(aX, bZ);

    float ryx = VectorDot(aY, bX);
    float ryy = VectorDot(aY, bY);
    float ryz = VectorDot(aY, bZ);

    float rzx = VectorDot(aZ, bX);
    float rzy = VectorDot(aZ, bY);
    float rzz = VectorDot(aZ, bZ);

    cubeA->collisionDetected = false;
    cubeB->collisionDetected = false;
    cubeA->rb->applyGravity = true;
    cubeB->rb->applyGravity = true;
    bool faceToFace = false;
    ContactInfo faceInfo;
    ContactInfo edgeInfo;
    float t1 = 0, t2 = 0;

    float lowestAPenetration = -std::numeric_limits<float>().max();
    float lowestBPenetration = -std::numeric_limits<float>().max();

    float penetration = fabsf(VectorDot(T, aX)) - (cubeA->xSize + fabsf(cubeB->xSize * rxx) + fabsf(cubeB->ySize * rxy) + fabsf(cubeB->zSize * rxz));
    //check for collisions parallel to AX
    if (penetration > 0)
    {
        return false;
    }
    faceInfo.penetrationDistance = penetration;
    faceInfo.normal = aX;
    faceInfo.faceCollision = true;
    faceInfo.aDir = BoxCollider::ContactDir::RIGHT;
    t1 += penetration;

    penetration = fabsf(VectorDot(T, aY)) - (cubeA->ySize + fabsf(cubeB->xSize * ryx) + fabsf(cubeB->ySize * ryy) + fabsf(cubeB->zSize * ryz));
    //check for collisions parallel to AY
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > faceInfo.penetrationDistance)
    {
        faceInfo.penetrationDistance = penetration;
        faceInfo.normal = aY;
        faceInfo.faceCollision = true;
        faceInfo.aDir = BoxCollider::ContactDir::UP;
    }
    t1 += penetration;

    penetration = fabsf(VectorDot(T, aZ)) - (cubeA->zSize + fabsf(cubeB->xSize * rzx) + fabsf(cubeB->ySize * rzy) + fabsf(cubeB->zSize * rzz));
    //check for collisions parallel to AZ
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > faceInfo.penetrationDistance)
    {
        faceInfo.penetrationDistance = penetration;
        faceInfo.normal = aZ;
        faceInfo.faceCollision = true;
        faceInfo.aDir = BoxCollider::ContactDir::FORWARD;
    }
    t1 += penetration;

    penetration = fabsf(VectorDot(T, bX)) - (fabsf(cubeA->xSize * rxx) + fabsf(cubeA->ySize * ryx) + fabsf(cubeA->zSize * rzx) + cubeB->xSize);
    //check for collisions parallel to BX
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > lowestBPenetration)
    {
        lowestBPenetration = penetration;

        faceInfo.faceCollision = true;
        faceInfo.bDir = BoxCollider::ContactDir::RIGHT;
        if (penetration > faceInfo.penetrationDistance)
        {
            faceInfo.penetrationDistance = penetration;
            faceInfo.normal = bX;
        }
    }

    penetration = fabsf(VectorDot(T, bY)) - (fabsf(cubeA->xSize * rxy) + fabsf(cubeA->ySize * ryy) + fabsf(cubeA->zSize * rzy) + cubeB->ySize);
    //check for collisions parallel to BY
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > lowestBPenetration)
    {
        lowestBPenetration = penetration;

        faceInfo.faceCollision = true;
        faceInfo.bDir = BoxCollider::ContactDir::UP;
        if (penetration > faceInfo.penetrationDistance)
        {
            faceInfo.penetrationDistance = penetration;
            faceInfo.normal = bY;
        }
    }

    penetration = fabsf(VectorDot(T, bZ)) - (fabsf(cubeA->xSize * rxz) + fabsf(cubeA->ySize * ryz) + fabsf(cubeA->zSize * rzz) + cubeB->zSize);
    //check for collisions parallel to BZ
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > lowestBPenetration)
    {
        lowestBPenetration = penetration;

        faceInfo.faceCollision = true;
        faceInfo.bDir = BoxCollider::ContactDir::FORWARD;
        if (penetration > faceInfo.penetrationDistance)
        {
            faceInfo.penetrationDistance = penetration;
            faceInfo.normal = bZ;
        }
    }

    penetration = fabsf(VectorDot(T, aZ) * ryx - VectorDot(T, aY) * rzx) - (fabsf(cubeA->ySize * rzx) + fabsf(cubeA->zSize * ryx) + fabsf(cubeB->ySize * rxz) + fabsf(cubeB->zSize * rxy));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aX, bX);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::RIGHT;
        edgeInfo.bDir = BoxCollider::ContactDir::RIGHT;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aZ) * ryy - VectorDot(T, aY) * rzy) - (fabsf(cubeA->ySize * rzy) + fabsf(cubeA->zSize * ryy) + fabsf(cubeB->xSize * rxz) + fabsf(cubeB->zSize * rxx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aX, bY);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::RIGHT;
        edgeInfo.bDir = BoxCollider::ContactDir::UP;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aZ) * ryz - VectorDot(T, aY) * rzz) - (fabsf(cubeA->ySize * rzz) + fabsf(cubeA->zSize * ryz) + fabsf(cubeB->xSize * rxy) + fabsf(cubeB->ySize * rxx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aX, bZ);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::RIGHT;
        edgeInfo.bDir = BoxCollider::ContactDir::FORWARD;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aX) * rzx - VectorDot(T, aZ) * rxx) - (fabsf(cubeA->xSize * rzx) + fabsf(cubeA->zSize * rxx) + fabsf(cubeB->ySize * ryz) + fabsf(cubeB->zSize * ryy));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aY, bX);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::UP;
        edgeInfo.bDir = BoxCollider::ContactDir::RIGHT;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aX) * rzy - VectorDot(T, aZ) * rxy) - (fabsf(cubeA->xSize * rzy) + fabsf(cubeA->zSize * rxy) + fabsf(cubeB->xSize * ryz) + fabsf(cubeB->zSize * ryx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aY, bY);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::UP;
        edgeInfo.bDir = BoxCollider::ContactDir::UP;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aX) * rzz - VectorDot(T, aZ) * rxz) - (fabsf(cubeA->xSize * rzz) + fabsf(cubeA->zSize * rxz) + fabsf(cubeB->xSize * ryy) + fabsf(cubeB->ySize * ryx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aY, bZ);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::UP;
        edgeInfo.bDir = BoxCollider::ContactDir::FORWARD;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aY) * rxx - VectorDot(T, aX) * ryx) - (fabsf(cubeA->xSize * ryx) + fabsf(cubeA->ySize * rxx) + fabsf(cubeB->ySize * rzz) + fabsf(cubeB->zSize * rzy));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aZ, bX);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::FORWARD;
        edgeInfo.bDir = BoxCollider::ContactDir::RIGHT;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aY) * rxy - VectorDot(T, aX) * ryy) - (fabsf(cubeA->xSize * ryy) + fabsf(cubeA->ySize * rxy) + fabsf(cubeB->xSize * rzz) + fabsf(cubeB->zSize * rzx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aZ, bY);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::FORWARD;
        edgeInfo.bDir = BoxCollider::ContactDir::UP;
    }
    t2 += penetration;

    penetration = fabsf(VectorDot(T, aY) * rxz - VectorDot(T, aX) * ryz) - (fabsf(cubeA->xSize * ryz) + fabsf(cubeA->ySize * rxz) + fabsf(cubeB->xSize * rzy) + fabsf(cubeB->ySize * rzx));
    if (penetration > 0)
    {
        return false;
    }
    if (penetration > edgeInfo.penetrationDistance && penetration < 0.0)
    {
        edgeInfo.penetrationDistance = penetration;
        edgeInfo.normal = VectorCross(aZ, bZ);
        edgeInfo.faceCollision = false;
        edgeInfo.aDir = BoxCollider::ContactDir::FORWARD;
        edgeInfo.bDir = BoxCollider::ContactDir::FORWARD;
    }
    t2 += penetration;



    if ((cubeA->rb->atRest && !cubeB->rb->atRest) && cubeB->rb->dynamic)
        cubeA->rb->atRest = false;
    if ((!cubeA->rb->atRest && cubeB->rb->atRest) && cubeA->rb->dynamic)
        cubeB->rb->atRest = false;

    faceInfo.normal = Sign(VectorDot(T, faceInfo.normal)) * faceInfo.normal;
    faceInfo.normal = VectorNormalize(faceInfo.normal);
    std::vector<Vector3> closestVertsA = cubeA->getClosestVerts(-faceInfo.normal);
    std::vector<Vector3> closestVertsB = cubeB->getClosestVerts(faceInfo.normal);
    bool facecollision = false;

    facecollision = isCubeCubePetrusion(-faceInfo.normal, closestVertsA, cubeB, faceInfo.bDir) || isCubeCubePetrusion(faceInfo.normal, closestVertsB, cubeA, faceInfo.aDir);
    // }
    //    else if(faceToFace)
    //    {
    //        //        facecollision = isCubeCubePetrusion(-faceInfo.normal, closestVertsA, cubeB, faceInfo.bDir)||
    //        //                isCubeCubePetrusion(faceInfo.normal, closestVertsB, cubeA, faceInfo.aDir);
    //        facecollision = true;

    //    }
    if (facecollision)
    {
        contactInfo = faceInfo;
    }
    else
    {
        //edgeInfo.normal = faceInfo.normal;
        contactInfo = edgeInfo;

    }

    contactInfo.a = cubeA;
    contactInfo.b = cubeB;


    cubeA->collisionDetected = true;
    cubeB->collisionDetected = true;
    return true;


}

void PhysicsWorld::determineCubeCubeContactPoints(ContactInfo& info, BoxCollider* cubeA, BoxCollider* cubeB)
{
    Vector3 T = cubeB->rb->position - cubeA->rb->position;
    info.normal = Sign(VectorDot(T, info.normal)) * info.normal;
    info.normal = VectorNormalize(info.normal);
    std::vector<Vector3> closestVertsA = cubeA->getClosestVerts(-info.normal);
    std::vector<Vector3> closestVertsB = cubeB->getClosestVerts(info.normal);


    std::vector<BoxCollider::EdgeIndices> aEdges = cubeA->getEdgesFromVertexIndices();
    std::vector<BoxCollider::EdgeIndices> bEdges = cubeB->getEdgesFromVertexIndices();
    float minDist = std::numeric_limits<float>().max();
    RayCastData data;
    for (BoxCollider::EdgeIndices ea : aEdges)
    {
        for (BoxCollider::EdgeIndices eb : bEdges)
        {
            //            Vector3 tangentialA = ea.dir*ea.length - VectorDot(ea.dir*ea.length, info.normal)*info.normal;
            //            float aLen = VectorLength(tangentialA);
            //            Vector3 tangentialB = eb.dir*eb.length - VectorDot(eb.dir*eb.length, info.normal)*info.normal;
            //            float bLen = VectorLength(tangentialB);
            //            tangentialA /= aLen;
            //            tangentialB /= bLen;

            //            if(closestPointsDoIntersect(ea.midPoint, eb.midPoint, ea.dir, eb.dir, ea.length, eb.length, ea.normalLength, eb.normalLength))
            //            {
            //                Vector3 testPoint = closestPointBetweenLines(ea.midPoint, eb.midPoint, ea.dir, eb.dir);
            //                Vector3 pa1 = ea.midPoint+ea.dir*ea.length;
            //                Vector3 pa2 = ea.midPoint-ea.dir*ea.length;
            //                Vector3 pb1 = eb.midPoint+eb.dir*eb.length;
            //                Vector3 pb2 = eb.midPoint-eb.dir*eb.length;
            //                if((cubeRaycast(pa1, -ea.dir, data, cubeB) || cubeRaycast(pa2, ea.dir, data, cubeB))&&(cubeRaycast(pb1, -eb.dir, data, cubeA) || cubeRaycast(pb2, eb.dir, data, cubeA)))
            //                {
            //                    float dist = closestDistanceBetweenLines(ea.midPoint, eb.midPoint, ea.dir, eb.dir, ea.length, eb.length);
            //                    if(dist<info.penetrationDistance && !info.faceCollision)
            //                        info.penetrationDistance = dist;
            //                    info.points.push_back(testPoint);
            //                    info.edgePoints++;
            //                }
            //            }

            //if(closestPointsDoIntersect(ea.midPoint, eb.midPoint, ea.dir, eb.dir, ea.length, eb.length, ea.normalLength, eb.normalLength))
            {
                Vector3 testPoint = closestPointBetweenLines(ea.midPoint, eb.midPoint, ea.dir, eb.dir);
                Vector3 pa1 = ea.midPoint + ea.dir * ea.length;
                Vector3 pa2 = ea.midPoint - ea.dir * ea.length;
                Vector3 pb1 = eb.midPoint + eb.dir * eb.length;
                Vector3 pb2 = eb.midPoint - eb.dir * eb.length;
                if (cubeRaycast(pa1, -ea.dir, data, cubeB) && data.length < ea.length)
                {
                    info.points.push_back(data.point);
                    info.edgePoints++;
                }
                if (cubeRaycast(pa2, ea.dir, data, cubeB) && data.length < ea.length)
                {

                    info.points.push_back(data.point);
                    info.edgePoints++;
                }
                if (cubeRaycast(pb1, -eb.dir, data, cubeA) && data.length < eb.length)
                {

                    info.points.push_back(data.point);
                    info.edgePoints++;
                }
                if (cubeRaycast(pb2, eb.dir, data, cubeA) && data.length < eb.length)
                {

                    info.points.push_back(data.point);
                    info.edgePoints++;
                }
            }
        }
    }


    if (info.faceCollision)
    {
        determineCubeCubePetrusionVerts(info, -info.normal, closestVertsA, cubeB, info.bDir, true);
        determineCubeCubePetrusionVerts(info, info.normal, closestVertsB, cubeA, info.aDir, true);
    }

    contacts.push_back(info);
}

bool PhysicsWorld::isCubeCubePetrusion(const Vector3& normal, const std::vector<Vector3>& points, BoxCollider* toCube, BoxCollider::ContactDir dir)
{
    Vector3 p0 = toCube->rb->position + toCube->rb->getLocalXAxis() * toCube->xSize;
    Vector3 adj1 = toCube->rb->getLocalYAxis();
    float maxDist1 = toCube->ySize;
    Vector3 adj2 = toCube->rb->getLocalZAxis();
    float maxDist2 = toCube->zSize;
    float tolerance = 0.005f;
    switch (dir)
    {
    case BoxCollider::ContactDir::RIGHT:
        if (VectorDot(normal, toCube->rb->getLocalXAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalXAxis() * toCube->xSize;
        break;
    case BoxCollider::ContactDir::UP:
        p0 = toCube->rb->position + toCube->rb->getLocalYAxis() * toCube->ySize;
        if (VectorDot(normal, toCube->rb->getLocalYAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalYAxis() * toCube->ySize;
        adj1 = toCube->rb->getLocalXAxis();
        maxDist1 = toCube->xSize;
        adj2 = toCube->rb->getLocalZAxis();
        maxDist2 = toCube->zSize;
        break;
    case BoxCollider::ContactDir::FORWARD:
        p0 = toCube->rb->position + toCube->rb->getLocalZAxis() * toCube->zSize;
        if (VectorDot(normal, toCube->rb->getLocalZAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalZAxis() * toCube->zSize;
        adj1 = toCube->rb->getLocalXAxis();
        maxDist1 = toCube->xSize;
        adj2 = toCube->rb->getLocalYAxis();
        maxDist2 = toCube->ySize;
        break;
    }

    for (Vector3 point : points)
    {
        float d = VectorDot(p0 - point, normal) / VectorDot(normal, normal);
        if (d >= -tolerance)
        {
            Vector3 intersectionPoint = point + d * normal;
            float dist1 = fabsf(VectorDot(intersectionPoint - p0, adj1));
            float dist2 = fabsf(VectorDot(intersectionPoint - p0, adj2));
            if (dist1 <= maxDist1 + tolerance && dist2 <= maxDist2 + tolerance)
                return true;
        }
    }
    return false;
}

bool PhysicsWorld::sphereRaycast(const Vector3& start, const Vector3& dir, RayCastData& dat, SphereCollider* sphere)
{
    Vector3 right(1, 0, 0), up(0, 1, 0);
    if (VectorEpsilonEqual(dir, Vector3(0, 1, 0), 0.005f) || VectorEpsilonEqual(dir, Vector3(0, -1, 0), 0.005f))
    {
        up = Vector3(0, 0, 1);
    }
    else
    {
        right = VectorNormalize(VectorCross(up, dir));
        up = VectorCross(right, dir);
    }

    bool hits = false;
    Vector3 p0 = sphere->rb->position;
    Vector3 normal = -dir;
    float dist = VectorDot((p0 - start), normal) / VectorDot(dir, normal);

    if (dist >= 0.0f)
    {
        Vector3 intersection = start + dir * dist;
        Vector3 radius = intersection - p0;
        float l2 = VectorLengthSquared(radius);
        if (l2 <= sphere->radius * sphere->radius)
        {
            hits = true;
            Vector3 rightComp = VectorDot(right, radius) * right;
            Vector3 upComp = VectorDot(up, radius) * up;
            float length = VectorLength(rightComp + upComp);
            float theta = asinf(length / sphere->radius);
            Vector3 dirComp = normal * length / tanf(theta);
            if (length == 0.0f)
                dirComp = normal * sphere->radius;
            Vector3 point = rightComp + upComp + dirComp + p0;
            dat.length = VectorLength(point - start);
            dat.normal = rightComp + upComp + dirComp;
            dat.point = point;
            dat.collider = sphere;
        }
    }

    return hits;
}

bool PhysicsWorld::cubeRaycast(const Vector3& start, const Vector3& dir, RayCastData& dat, BoxCollider* cube)
{
    float minDist = std::numeric_limits<float>().max();
    //normal of the plane
    Vector3 normal;
    //initial points on plane
    Vector3 p0;
    //The adjacent directions to check if its on the plane
    Vector3 adj1;
    float maxDist1;
    Vector3 adj2;
    float maxDist2;
    bool doesHit = false;

    //start with the left and right faces
    for (int i = 0; i < 6; i++)
    {
        switch (i)
        {
        case 0:
            normal = -cube->rb->getLocalXAxis();
            p0 = cube->rb->position - cube->xSize * cube->rb->getLocalXAxis();
            adj1 = cube->rb->getLocalYAxis();
            adj2 = cube->rb->getLocalZAxis();
            maxDist1 = cube->ySize;
            maxDist2 = cube->zSize;
            break;
        case 1:
            normal = cube->rb->getLocalXAxis();
            p0 = cube->rb->position + cube->xSize * cube->rb->getLocalXAxis();
            adj1 = cube->rb->getLocalYAxis();
            adj2 = cube->rb->getLocalZAxis();
            maxDist1 = cube->ySize;
            maxDist2 = cube->zSize;
            break;
        case 2:
            normal = -cube->rb->getLocalYAxis();
            p0 = cube->rb->position - cube->ySize * cube->rb->getLocalYAxis();
            adj1 = cube->rb->getLocalXAxis();
            adj2 = cube->rb->getLocalZAxis();
            maxDist1 = cube->xSize;
            maxDist2 = cube->zSize;
            break;
        case 3:
            normal = cube->rb->getLocalYAxis();
            p0 = cube->rb->position + cube->ySize * cube->rb->getLocalYAxis();
            adj1 = cube->rb->getLocalXAxis();
            adj2 = cube->rb->getLocalZAxis();
            maxDist1 = cube->xSize;
            maxDist2 = cube->zSize;
            break;
        case 4:
            normal = -cube->rb->getLocalZAxis();
            p0 = cube->rb->position - cube->zSize * cube->rb->getLocalZAxis();
            adj1 = cube->rb->getLocalYAxis();
            adj2 = cube->rb->getLocalXAxis();
            maxDist1 = cube->ySize;
            maxDist2 = cube->xSize;
            break;
        case 5:
            normal = cube->rb->getLocalZAxis();
            p0 = cube->rb->position + cube->zSize * cube->rb->getLocalZAxis();
            adj1 = cube->rb->getLocalYAxis();
            adj2 = cube->rb->getLocalXAxis();
            maxDist1 = cube->ySize;
            maxDist2 = cube->xSize;
            break;
        }

        float dist = VectorDot((p0 - start), normal) / VectorDot(dir, normal);
        if (dist >= 0.0f)
        {
            Vector3 intersection = start + dir * dist;
            float dist1 = fabsf(VectorDot(intersection - p0, adj1));
            float dist2 = fabsf(VectorDot(intersection - p0, adj2));
            if (dist1 <= maxDist1 && dist2 <= maxDist2)
            {
                //cast hits, check if its the minimum
                if (dist < minDist)
                {
                    minDist = dist;
                    dat.normal = normal;
                    dat.point = intersection;
                    dat.length = dist;
                    doesHit = true;
                    dat.collider = cube;
                }
            }

        }
    }

    return doesHit;
}

void PhysicsWorld::determineCubeCubePetrusionVerts(ContactInfo& info, const Vector3& normal, const std::vector<Vector3>& points, BoxCollider* toCube, BoxCollider::ContactDir dir, bool adjustPenetration)
{
    Vector3 p0 = toCube->rb->position + toCube->rb->getLocalXAxis() * toCube->xSize;
    Vector3 adj1 = toCube->rb->getLocalYAxis();
    float maxDist1 = toCube->ySize;
    Vector3 adj2 = toCube->rb->getLocalZAxis();
    float maxDist2 = toCube->zSize;
    switch (dir)
    {
    case BoxCollider::ContactDir::RIGHT:
        if (VectorDot(normal, toCube->rb->getLocalXAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalXAxis() * toCube->xSize;
        break;
    case BoxCollider::ContactDir::UP:
        p0 = toCube->rb->position + toCube->rb->getLocalYAxis() * toCube->ySize;
        if (VectorDot(normal, toCube->rb->getLocalYAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalYAxis() * toCube->ySize;
        adj1 = toCube->rb->getLocalXAxis();
        maxDist1 = toCube->xSize;
        adj2 = toCube->rb->getLocalZAxis();
        maxDist2 = toCube->zSize;
        break;
    case BoxCollider::ContactDir::FORWARD:
        p0 = toCube->rb->position + toCube->rb->getLocalZAxis() * toCube->zSize;
        if (VectorDot(normal, toCube->rb->getLocalZAxis()) < 0)
            p0 = toCube->rb->position - toCube->rb->getLocalZAxis() * toCube->zSize;
        adj1 = toCube->rb->getLocalXAxis();
        maxDist1 = toCube->xSize;
        adj2 = toCube->rb->getLocalYAxis();
        maxDist2 = toCube->ySize;
        break;
    }

    for (Vector3 point : points)
    {
        float d = VectorDot(p0 - point, normal) / VectorDot(normal, normal);
        //This now becomes the new penetration distance
        //qDebug() << "Penetration distance: " << d;

        if (d >= 0.0f)
        {
            Vector3 intersectionPoint = point + d * normal;
            float dist1 = fabsf(VectorDot(intersectionPoint - p0, adj1));
            float dist2 = fabsf(VectorDot(intersectionPoint - p0, adj2));
            if (dist1 <= maxDist1 && dist2 <= maxDist2)
            {
                info.points.push_back(intersectionPoint);
                if (adjustPenetration && d >= 0.0f && d >= -info.penetrationDistance)
                {
                    info.penetrationDistance = -d;
                    info.vertexPoints = info.points.size() - 1;
                }

            }
        }
    }
}

void PhysicsWorld::cubeCubeCollisionResponse(ContactInfo& info, float dt, BoxCollider* cubeA, BoxCollider* cubeB)
{

    cubeA->rb->position += 0.5f * info.normal * info.penetrationDistance;
    cubeB->rb->position -= 0.5f * info.normal * info.penetrationDistance;


    for (int i = 0; i < info.points.size(); i++)
    {
        float epsilon = 0.5f;
        Vector3 ra = info.points[i] - cubeA->rb->position;
        Vector3 rb = info.points[i] - cubeB->rb->position;
        Vector3 va = cubeA->rb->velocity + VectorCross(cubeA->rb->angularVelocity, ra);
        Vector3 vb = cubeB->rb->velocity + VectorCross(cubeB->rb->angularVelocity, rb);
        float vRel = VectorDot(info.normal, va - vb);

        //Vector3 vPerp = VectorNormalize(VectorCross(info.normal, va-vb));
        //qDebug() << "vPerp x:" << vPerp.x << " y: " << vPerp.y << " z: " << vPerp.z;
        float numerator = -(1 - epsilon) * vRel;

        float t1 = cubeA->rb->massInv;
        float t2 = cubeB->rb->massInv;
        float t3 = VectorDot(info.normal, VectorCross(VectorCross(cubeA->rb->inertiaInv * ra, info.normal), ra));
        float t4 = VectorDot(info.normal, VectorCross(VectorCross(cubeB->rb->inertiaInv * rb, info.normal), rb));

        float j = numerator / (t1 + t2 + t3 + t4);
        Vector3 force = j * info.normal / (dt * info.points.size());
        float angularRel = VectorLength(cubeA->rb->angularVelocity - cubeB->rb->angularVelocity);


        if (cubeA->rb->dynamic)
        {
            Vector3 perpVelNormal = VectorNormalize(cubeA->rb->velocity - va * info.normal);
            if (!cubeB->rb->dynamic) {
                Vector3 fric = friction * perpVelNormal * cubeA->rb->mass * VectorDot(gravity, info.normal);
                force -= fric;
            }
            cubeA->rb->addForce(force, *cubeB->rb);
            cubeA->rb->addTorque(VectorCross(ra, force));
        }
        if (cubeB->rb->dynamic)
        {
            Vector3 perpVelNormal = VectorNormalize(cubeB->rb->velocity + vb * info.normal);
            if (!cubeA->rb->dynamic)
            {
                Vector3 fric = friction * perpVelNormal * cubeB->rb->mass * VectorDot(gravity, info.normal);
                force -= fric;
            }
            cubeB->rb->addForce(-force, *cubeA->rb);
            cubeB->rb->addTorque(-VectorCross(rb, force));
        }
        //qDebug() << "not resting";
    }

}

Vector3 landingHorizontalSpeed;
Vector3 frictionalSpeed;
void PhysicsWorld::cubeCubeCollisionResponseDynamicVsStatic(ContactInfo& info, const Vector3& norm, float dt, BoxCollider* dynamicCube, BoxCollider* staticCube)
{

    if (info.faceCollision)
        dynamicCube->rb->position += norm * info.penetrationDistance;

    if (!dynamicCube->rb->restingContact)
    {
        // qDebug() << "not stabalizing";
        // if(info.points.size()==0)
        //dynamicCube->rb->restingContact = true;
        for (int i = 0; i < info.points.size(); i++)
        {
            float epsilon = 0.5f;
            Vector3 ra = info.points[i] - dynamicCube->rb->position;
            Vector3 rb = info.points[i] - staticCube->rb->position;
            Vector3 va = dynamicCube->rb->velocity + VectorCross(dynamicCube->rb->angularVelocity, ra);
            float vRel = VectorDot(info.normal, va);

            float numerator = -(1 - epsilon) * vRel;

            float t1 = dynamicCube->rb->massInv;
            float t2 = staticCube->rb->massInv;
            float t3 = VectorDot(info.normal, VectorCross(VectorCross(dynamicCube->rb->inertiaInv * ra, info.normal), ra));
            float t4 = VectorDot(info.normal, VectorCross(VectorCross(staticCube->rb->inertiaInv * rb, info.normal), rb));

            float j = numerator / (t1 + t2 + t3 + t4);
            Vector3 force = j * info.normal / (dt * info.points.size());
            float angularRel = VectorLength(dynamicCube->rb->angularVelocity - staticCube->rb->angularVelocity);


            // qDebug() << "surface slope: " << fabsf(VectorDot(VectorNormalize(gravity), info.normal));
            if (fabsf(j) < 0.5f && fabsf(VectorDot(VectorNormalize(gravity), info.normal)) > 0.7f)
            {
                dynamicCube->rb->restingContact = true;
                landingHorizontalSpeed = dynamicCube->rb->velocity - VectorDot(dynamicCube->rb->velocity, norm) * norm;

            }

            dynamicCube->rb->addForce(force, *dynamicCube->rb);
            dynamicCube->rb->addTorque(VectorCross(ra, force));
        }
    }
    else
    {
        //Handle the behaviour of resting contact

        float angularSpeed = VectorLength(dynamicCube->rb->angularVelocity);
        float speed = VectorLength(dynamicCube->rb->velocity);

        if (speed < 0.1f && angularSpeed < 0.1f)
        {
            bool reverseDir;
            Vector3 faceDir;
            BoxCollider::ContactDir upDir = dynamicCube->GetFaceClosestToNormal(info.normal, reverseDir, faceDir);
            if (fabsf(VectorDot(faceDir, info.normal)) > 0.975f)
            {
                dynamicCube->rb->restingContact = false;
                dynamicCube->rb->atRest = true;
                Vector3 up = info.normal;
                if (reverseDir)
                    up = -up;
                Quaternion toRotation = dynamicCube->toRotation(upDir, up);
                dynamicCube->rb->rotation = toRotation;
            }

        }
        else if (info.points.size())
        {

            Vector3 totalVelocity(0, 0, 0);
            for (int i = 0; i < info.points.size(); i++)
            {
                float epsilon = 0.25f;
                Vector3 ra = info.points[i] - dynamicCube->rb->position;
                Vector3 rb = info.points[i] - staticCube->rb->position;
                Vector3 va = dynamicCube->rb->velocity + VectorCross(dynamicCube->rb->angularVelocity, ra);
                Vector3 vn = VectorDot(va, info.normal) * info.normal;
                Vector3 vt = va - vn;
                float vRel = VectorDot(info.normal, va);

                float numerator = -(1 - epsilon) * vRel;

                float t1 = dynamicCube->rb->massInv;
                float t2 = staticCube->rb->massInv;
                float t3 = VectorDot(info.normal, VectorCross(VectorCross(dynamicCube->rb->inertiaInv * ra, info.normal), ra));
                float t4 = VectorDot(info.normal, VectorCross(VectorCross(staticCube->rb->inertiaInv * rb, info.normal), rb));

                float j = numerator / (t1 + t2 + t3 + t4);
                Vector3 normalForce = j * info.normal / (dt * info.points.size());
                Vector3 tangentialForce = (VectorLength(vt) / VectorLength(vn)) * fabsf(j) * VectorNormalize(vt) / (dt * info.points.size());
                // Utilities::PrintVec3(tangentialForce);
                float angularRel = VectorLength(dynamicCube->rb->angularVelocity - staticCube->rb->angularVelocity);

                //if(info.faceCollision)
                dynamicCube->rb->addTorque(10.0f * VectorCross(dynamicCube->rb->mass * gravity, ra));
                if (j > 0)
                    dynamicCube->rb->addTorque(VectorCross(ra, normalForce));

                Vector3 rotationPoint = info.points[i];
                Vector3 radius = dynamicCube->rb->position - rotationPoint;



                Vector3 velocityFromAngular = VectorCross(dynamicCube->rb->angularVelocity, radius);
                velocityFromAngular -= VectorDot(velocityFromAngular, norm) * norm;

                Vector3 torsion = VectorDot(dynamicCube->rb->mass * gravity, norm) * VectorDot(dynamicCube->rb->angularVelocity, norm) * norm;
                dynamicCube->rb->addTorque(-torsion);

                if (VectorLength(vt) < 0.1f)
                    vt = Vector3(0, 0, 0);
                else
                    vt = vt - VectorNormalize(vt) * friction * 0.4f;
                totalVelocity += velocityFromAngular + vt;
            }
            totalVelocity =  totalVelocity/(float)info.points.size();
            if (!allNaN(totalVelocity))
                dynamicCube->rb->setVelocity(totalVelocity);
        }
    }

}

void PhysicsWorld::cubeSphereCollisionResponseDynamicVsDynamic(ContactInfo& info, float dt, BoxCollider* cube, SphereCollider* sphere)
{
    // qDebug()<<"dynamic response:";

    //cube->rb->position += info.normal*info.penetrationDistance;
    //if(info.faceCollision)
    {
        sphere->rb->position -= 0.5f * info.normal * info.penetrationDistance;
        cube->rb->position += 0.5f * info.normal * info.penetrationDistance;
    }

    float epsilon = 0.5f;
    Vector3 ra = info.points[0] - cube->rb->position;
    Vector3 rb = info.points[0] - sphere->rb->position;
    Vector3 va = cube->rb->velocity + VectorCross(cube->rb->angularVelocity, ra);
    Vector3 vb = sphere->rb->velocity + VectorCross(sphere->rb->angularVelocity, rb);
    float vRel = VectorDot(info.normal, va - vb);

    float numerator = -(1 - epsilon) * vRel;

    float t1 = cube->rb->massInv;
    float t2 = sphere->rb->massInv;
    float t3 = VectorDot(info.normal, VectorCross(VectorCross(cube->rb->inertiaInv * ra, info.normal), ra));
    float t4 = VectorDot(info.normal, VectorCross(VectorCross(sphere->rb->inertiaInv * rb, info.normal), rb));

    float j = numerator / (t1 + t2 + t3 + t4);
    Vector3 force = j * info.normal / dt;

    if (j < 0)
    {
        cube->rb->addForce(force, *cube->rb);
        cube->rb->addTorque(VectorCross(ra, force));

        sphere->rb->addForce(-force, *sphere->rb);
        sphere->rb->addTorque(VectorCross(rb, -force));
    }

}

void PhysicsWorld::cubeSphereCollisionResponseStaticVsDynamic(ContactInfo& info, float dt, BoxCollider* cube, SphereCollider* sphere)
{
    Vector3 normal = info.normal;
    sphere->rb->position -= normal * info.penetrationDistance;
    Vector3 r = info.points[0] - sphere->rb->position;

    Vector3 vn = VectorDot(normal, sphere->rb->velocity) * sphere->rb->velocity;
    Vector3 vt = sphere->rb->velocity - vn;

    Vector3 force = -1.1f * normal * VectorDot(sphere->rb->velocity, normal) * sphere->rb->mass / dt;
    float j = VectorLength(force);
    float sMax = 5.0f * restitutionSlope * fabsf(VectorDot(gravity, normal)) + restitutionIntersect;
    float slope = VectorDot(info.normal, Vector3(0, 1, 0));

    if (j > sMax || slope < 0.0f)
    {

        sphere->rb->addForce(force);
        Vector3 forceT = VectorNormalize(vt) * VectorLength(vt) / VectorLength(vn);
        sphere->rb->setAngularVelocity(VectorCross(r, -vt / sphere->radius));
    }
    else
    {
        //if(info.faceCollision)
        {
            sphere->rb->linearMomentum = VectorCross(VectorCross(normal, sphere->rb->linearMomentum), normal);
            sphere->rb->setAngularVelocity(VectorCross(normal, sphere->rb->velocity / sphere->radius));
            sphere->rb->addForce(friction * VectorNormalize(sphere->rb->velocity) * VectorDot(gravity, normal));
        }
        //        else
        //        {
        //            //if(j<0)
        //                sphere->rb->addForce(force);
        //        }
    }

}

bool PhysicsWorld::detectSphereSphereCollision(SphereCollider* sphere, SphereCollider* other)
{
    Vector3 dp = other->rb->position - sphere->rb->position;
    float lSquared = VectorLengthSquared(dp);
    float minDist = other->radius + sphere->radius;
    bool result = lSquared <= minDist * minDist;
    sphere->collisionDetected = result;
    other->collisionDetected = result;
    return result;
}

void PhysicsWorld::sphereSphereCollisionResponse(float dt, SphereCollider* sphere, SphereCollider* other)
{

    Vector3 dp = other->rb->position - sphere->rb->position;
    Vector3 relativeMomentum = sphere->rb->linearMomentum - other->rb->linearMomentum;
    dp = VectorNormalize(dp);
    float mag = VectorDot(dp, relativeMomentum);
    if (mag > 0)
    {
        Vector3 relativeMomentumNorm = mag * dp;
        other->rb->addForce((1.0f / dt) * relativeMomentumNorm);
    }
}

bool PhysicsWorld::raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat)
{
    float minDist = std::numeric_limits<float>().max();
    RayCastData tempData;
    bool didHit = false;
    for (Collider* collider : colliders)
    {
        switch (collider->type)
        {
        case ColliderType::CUBE:
        {
            BoxCollider* cube = dynamic_cast<BoxCollider*>(collider);
            if (cubeRaycast(start, dir, tempData, cube))
            {
                didHit = true;
                if (tempData.length < minDist)
                {
                    dat = tempData;
                    minDist = tempData.length;
                }
            }
        }
        break;

        case ColliderType::SPHERE:
        {
            SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider);
            if (sphereRaycast(start, dir, tempData, sphere))
            {
                didHit = true;
                if (tempData.length < minDist)
                {
                    dat = tempData;
                    minDist = tempData.length;
                }
            }
        }
        break;
        }
    }

    return didHit;
}
bool PhysicsWorld::raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat, int mask)
{
    return false;
}
bool PhysicsWorld::raycastAll(const Vector3& start, const Vector3& dir, RayCastData& dat, ColliderType type)
{
    float minDist = std::numeric_limits<float>().max();
    RayCastData tempData;
    bool didHit = false;
    for (Collider* collider : colliders)
    {
        if (collider->type == type)
        {
            switch (type)
            {
            case ColliderType::CUBE:
            {
                BoxCollider* cube = dynamic_cast<BoxCollider*>(collider);
                if (cubeRaycast(start, dir, tempData, cube))
                {
                    didHit = true;
                    if (tempData.length < minDist)
                    {
                        dat = tempData;
                        minDist = tempData.length;
                    }
                }
            }
            break;

            case ColliderType::SPHERE:
            {
                SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider);
                if (sphereRaycast(start, dir, tempData, sphere))
                {
                    didHit = true;
                    if (tempData.length < minDist)
                    {
                        dat = tempData;
                        minDist = tempData.length;
                    }
                }
            }
            break;
            }
        }
    }

    return didHit;
}


void PhysicsWorld::updateQuantities(float dt)
{
    for (const auto& collider : colliders)
    {
        if (collider->rb != nullptr)
            collider->rb->stepQuantities(dt);
    }
}
void PhysicsWorld::stepWorld(float dt)
{
    checkForCollisions(dt);
    updateQuantities(dt);
}

void PhysicsWorld::stepWorld(float dt, int inc)
{
    float adjustedDt = dt / inc;
    for (int i = 0; i < inc; i++)
    {
        checkForCollisions(adjustedDt);
        updateQuantities(adjustedDt);
    }
}

bool PhysicsWorld::cubeFlatOnSurface(BoxCollider* cube, Vector3& normal, float tolerance)
{
    return true;
}
