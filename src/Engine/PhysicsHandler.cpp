//
//  PhysicsHandler.cpp
//  game_engine
//
//  Created by Jacob Robinson on 3/29/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>
#include <iostream>

#include "PhysicsHandler.h"
#include "Actor.h"

// ContactListener Class
// Called whenever 2 collisions come into contact
void ContactListener::BeginContact(b2Contact* contact)
{
    Actor* ActorA = reinterpret_cast<Actor*>(contact->GetFixtureA()->GetUserData().pointer);
    Actor* ActorB = reinterpret_cast<Actor*>(contact->GetFixtureB()->GetUserData().pointer);

    if (ActorA == nullptr || ActorB == nullptr) { return; }
    
    Collision col;
    col.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();

    // Trigger contact
    if (contact->GetFixtureA()->IsSensor())
    {
        col.point = b2Vec2(-999.0f, -999.0f);
        col.normal = b2Vec2(-999.0f, -999.0f);

        // Calls for fixture A
        col.other = ActorB;
        ActorA->TriggerEnter(col);

        // Calls for fixture B
        col.other = ActorA;
        ActorB->TriggerEnter(col);
    }
    // Collider contact
    else
    {
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        col.point = worldManifold.points[0];
        col.normal = worldManifold.normal;

        // Calls for fixture A
        col.other = ActorB;
        ActorA->CollisionEnter(col);

        // Calls for fixture B
        col.other = ActorA;
        ActorB->CollisionEnter(col);
    }
}

// Called whenever 2 collisions stop touching
void ContactListener::EndContact(b2Contact* contact)
{
    Actor* ActorA = reinterpret_cast<Actor*>(contact->GetFixtureA()->GetUserData().pointer);
    Actor* ActorB = reinterpret_cast<Actor*>(contact->GetFixtureB()->GetUserData().pointer);

    if (ActorA == nullptr || ActorB == nullptr) { return; }

    Collision col;
    col.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();
    col.point = b2Vec2(-999.0f, -999.0f);
    col.normal = b2Vec2(-999.0f, -999.0f);

    // Trigger contact
    if (contact->GetFixtureA()->IsSensor())
    {
        // Calls for fixture A
        col.other = ActorB;
        ActorA->TriggerExit(col);

        // Calls for fixture B
        col.other = ActorA;
        ActorB->TriggerExit(col);
    }
    // Collider contact
    else
    {
        // Calls for fixture A
        col.other = ActorB;
        ActorA->CollisionExit(col);

        // Calls for fixture B
        col.other = ActorA;
        ActorB->CollisionExit(col);
    }
}

// Raycast Callback Class
float RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
    Actor* actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
    
    if (actor == nullptr) {return -1.0f;}
    
    HitResult* result = new HitResult;
    result->point = point;
    result->actor = actor;
    result->normal = normal;
    result->is_trigger = fixture->IsSensor();
    
    results[fraction] = result;
    
    return 1.0f;
}

// RayCaster Class
// Perform a Raycast with Box2D and return the first actor hit on the path of the ray.
HitResult* RayCaster::Raycast(b2Vec2 pos, b2Vec2 dir, float dist)
{
    // Return null if physics aren't active or the distance is 0
    if (dist <= 0.0f) {return luabridge::LuaRef(ComponentDB::luaState);}
    if (!PhysicsHandler::phyicsActive) {return luabridge::LuaRef(ComponentDB::luaState);}
    
    RayCastCallback callback;
    PhysicsHandler::world->RayCast(&callback, pos, pos + (dir.operator_mul(dist)));
    
    // Return null if the cast doesn't hit anything
    if (callback.results.empty()) {return luabridge::LuaRef(ComponentDB::luaState);}
    
    return callback.results.begin()->second;
}

// In this function, we return all hits (fixtures) that occur during the raycast operation, sorted by distance along the raycast (nearest to furthest).
luabridge::LuaRef RayCaster::RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
    // Creates and gets a reference to a new table on the Lua stack
    lua_newtable(ComponentDB::luaState);
    luabridge::LuaRef hitResults(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
    
    // Return null if physics aren't active or the distance is 0
    if (dist <= 0.0f) {return hitResults;}
    if (!PhysicsHandler::phyicsActive) {return hitResults;}
    
    RayCastCallback callback;
    PhysicsHandler::world->RayCast(&callback, pos, pos + (dir.operator_mul(dist)));
    
    // Return null if the cast doesn't hit anything
    if (callback.results.empty()) {return hitResults;}
    
    hitResults[0] = luabridge::LuaRef(ComponentDB::luaState);
    int i = 1;
    
    for (auto& member : callback.results)
    {
        hitResults[i] = member.second;
        i++;
    }
    
    return hitResults;
}

// Rigidbody Class
// The physics world
b2World* PhysicsHandler::world;

// Verifies if physics should be on
bool PhysicsHandler::phyicsActive = false;

// Initializes the physics world
void PhysicsHandler::Init()
{
    // Don't initialize the physics world more than once
    if (phyicsActive) {return;}
    
    phyicsActive = true;
    
    // Vector representing the direction and magnitude of gravity
    b2Vec2 gravity;
    
    gravity.x = 0.0f;
    gravity.y = 9.8f;
    
    world = new b2World(gravity);
    
    world->SetContactListener(new ContactListener());
}

// Steps forwards in the physics engine by one frame
void PhysicsHandler::Step()
{
    // Don't advance the physics world if it hasn't been initialized
    if (!phyicsActive) {return;}
    
    world->Step(1.0f / 60.0f, 8, 3);
}

// Rigidbody:
void Rigidbody::AddForce(b2Vec2 force) {body->ApplyForceToCenter(force, true);}

// Setters
void Rigidbody::SetVelocity(b2Vec2 velocity) {body->SetLinearVelocity(velocity);}
void Rigidbody::SetPosition(b2Vec2 position) 
{
    if (body == nullptr)
    {
        x = position.x;
        y = position.y;
        return;
    }
    body->SetTransform(position, GetRotation());
}
void Rigidbody::SetRotation(float degrees_clockwise) 
{
    if (body == nullptr)
    {
        rotation = degrees_clockwise;
        return;
    }
    body->SetTransform(GetPosition(), (static_cast<int>(degrees_clockwise) * (b2_pi / 180)));
}
void Rigidbody::SetAngularVelocity(float degrees_clockwise) {body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180));}
void Rigidbody::SetGravityScale(float gravity)
{
    if (body == nullptr)
    {
        gravity_scale = gravity;
        return;
    }
    body->SetGravityScale(gravity);
}
void Rigidbody::SetUpDirection(b2Vec2 direction)
{
    direction.Normalize();
    float rotationAngle = glm::atan(direction.x, -direction.y) - body->GetAngle();
    body->SetTransform(GetPosition(), body->GetAngle() + rotationAngle);
}
void Rigidbody::SetRightDirection(b2Vec2 direction)
{
    direction.Normalize();
    float rotationAngle = glm::atan(direction.x, -direction.y) - body->GetAngle();
    body->SetTransform(GetPosition(), (body->GetAngle() + rotationAngle) - (b2_pi / 2.0f));
}

b2Vec2 Rigidbody::GetPosition()
{
    if (body == nullptr) {return b2Vec2(x, y);}
    
    return body->GetPosition();
}

float Rigidbody::GetRotation()
{
    if (body == nullptr) {return rotation;}
    
    return body->GetAngle() * (180 / b2_pi);
}

b2Vec2 Rigidbody::GetVelocity() {return body->GetLinearVelocity();}
float Rigidbody::GetAngularVelocity() {return body->GetAngularVelocity() * (180 / b2_pi);}
float Rigidbody::GetGravityScale() 
{
    if (body == nullptr)
    {
        return gravity_scale;
    }
    return body->GetGravityScale();
}
b2Vec2 Rigidbody::GetUpDirection() {return b2Vec2(glm::sin(body->GetAngle()), -glm::cos(body->GetAngle()));}
b2Vec2 Rigidbody::GetRightDirection() {return -b2Vec2(glm::sin(body->GetAngle() - (b2_pi / 2.0f)), -glm::cos(body->GetAngle() - (b2_pi / 2.0f)));;}

void Rigidbody::OnStart()
{
    PhysicsHandler::Init();
    
    b2BodyDef bodyDef;
    
    // Sets the body type
    switch (body_type[0])
    {
        case 'd':
            bodyDef.type = b2_dynamicBody;
            break;
            
        case 's':
            bodyDef.type = b2_staticBody;
            break;
            
        case 'k':
            bodyDef.type = b2_kinematicBody;
            break;
            
        default:
            break;
    }
    
    // Physics settings
    bodyDef.bullet = precise;
    bodyDef.angularDamping = angular_friction;
    bodyDef.gravityScale = gravity_scale;
    
    // Transform settings
    bodyDef.position.x = x;
    bodyDef.position.y = y;
    bodyDef.angle = rotation * (b2_pi / 180);
    
    body = PhysicsHandler::world->CreateBody(&bodyDef);
   
    // Gives the body a shape in the world
    /* phantom sensor to make bodies move if neither collider nor trigger is present */
    if (!has_collider && !has_trigger)
    {
        b2FixtureDef* fixture = new b2FixtureDef;
        // Determines what this rigidbody can collide with
        fixture->filter.categoryBits = 0x0000;
        fixture->filter.maskBits = 0x0000;
        
        b2PolygonShape phantom_shape;
        phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);
        
        fixture->shape = &phantom_shape;
        
        // Because it is a sensor (with no callback even), no collisions will ever occur
        fixture->isSensor = true;
        fixture->density = density;

        body->CreateFixture(fixture);
    }

    // Creates a collider
    if (has_collider)
    {
        b2FixtureDef* fixture = new b2FixtureDef;
        // Determines what this rigidbody can collide with
        fixture->filter.categoryBits = 0x0001;
        fixture->filter.maskBits = 0x0001 + 0x0004;

        if (collider_type == "box")
        {
            b2PolygonShape* box = new b2PolygonShape;
            box->SetAsBox(width * 0.5f, height * 0.5f);
            fixture->shape = box;
        }
        else if (collider_type == "circle")
        {
            b2CircleShape* circle = new b2CircleShape;
            circle->m_radius = radius;
            fixture->shape = circle;
        }
    
        fixture->isSensor = false;
        fixture->density = density;
        fixture->friction = friction;
        fixture->restitution = bounciness;

        fixture->userData.pointer = reinterpret_cast<uintptr_t>(actor);

        body->CreateFixture(fixture);
    }

    // Creates a trigger
    if (has_trigger)
    {
        b2FixtureDef* fixture = new b2FixtureDef;
        // Determines what this rigidbody can collide with
        fixture->filter.categoryBits = 0x0002;
        fixture->filter.maskBits = 0x0002;

        if (trigger_type == "box")
        {
            b2PolygonShape* box = new b2PolygonShape;
            box->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
            fixture->shape = box;
        }
        else if (trigger_type == "circle")
        {
            b2CircleShape* circle = new b2CircleShape;
            circle->m_radius = trigger_radius;
            fixture->shape = circle;
        }

        fixture->isSensor = true;
        fixture->density = density;

        fixture->userData.pointer = reinterpret_cast<uintptr_t>(actor);

        body->CreateFixture(fixture);
    }
}

// Called this component is removed from its actor
void Rigidbody::OnDestroy()
{
    PhysicsHandler::world->DestroyBody(body);
}
