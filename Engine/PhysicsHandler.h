//
//  PhysicsHandler.h
//  game_engine
//
//  Created by Jacob Robinson on 3/29/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef PhysicsHandler_h
#define PhysicsHandler_h

#include <string>
#include <math.h>
#include <unordered_set>
#include <map>

#include "Box2D/box2d.h"
#include "glm/glm.hpp"

#include "lua.hpp"
#include "LuaBridge.h"

using namespace std;

class Actor;

struct Collision
{
    Actor* other;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

struct HitResult
{
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;
};

class ContactListener : public b2ContactListener
{
public:
    // Called whenever 2 collisions come into contact
    void BeginContact(b2Contact* contact);
    
    // Called whenever 2 collisions stop touching
    void EndContact(b2Contact* contact);
};


class RayCastCallback : public b2RayCastCallback
{
public:
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction);
    
    std::map<float, HitResult*> results;
};

class RayCaster
{
public:
    // Perform a Raycast with Box2D and return the first actor hit on the path of the ray.
    static HitResult* Raycast(b2Vec2 pos, b2Vec2 dir, float dist);
    
    // In this function, we return all hits (fixtures) that occur during the raycast operation, sorted by distance along the raycast (nearest to furthest).
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);
};

class Rigidbody
{
public:
    // Base component values
    std::string type = "Rigidbody";
    std::string key = "???";
    Actor* actor = nullptr;
    bool enabled = true;
    bool started = false;
    
    // Rigidbody values
    float x = 0.0f;
    float y = 0.0f;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f;
    float density = 1.0f;
    float angular_friction = 0.3f;
    float rotation = 0.0f;
    
    // Collider
    bool has_collider = true;
    std::string collider_type = "box";
    float width = 1.0f;
    float height = 1.0f;
    float radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;
    
    // Trigger
    bool has_trigger = true;
    std::string trigger_type = "box";
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    
    b2Body* body = nullptr;
    
    // Functions
    void AddForce(b2Vec2 force);
    
    // Setters
    void SetVelocity(b2Vec2 velocity);
    void SetPosition(b2Vec2 position);
    void SetRotation(float degrees_clockwise);
    void SetAngularVelocity(float degrees_clockwise);
    void SetGravityScale(float gravity);
    void SetUpDirection(b2Vec2 direction);
    void SetRightDirection(b2Vec2 direction);
    
    // Getters
    b2Vec2 GetPosition();
    float GetRotation();
    b2Vec2 GetVelocity();
    float GetAngularVelocity();
    float GetGravityScale();
    b2Vec2 GetUpDirection();
    b2Vec2 GetRightDirection();
    
    void OnStart();
    void OnDestroy();
};

class PhysicsHandler
{
public:
    // The physics world
    static b2World* world;
    
    // Verifies if physics should be on
    static bool phyicsActive;
    
    // Initializes the physics world
    static void Init();
    
    // Steps forwards in the physics engine by one frame
    static void Step();
};

#endif /* PhysicsHandler_h */
