#pragma once
#include "./protocol.h"
#include "../projects/spacegame/code/spaceship.h"
#include <gtx/quaternion.hpp>


namespace Game 
{
    class SpaceShip; // forward declaration
}
class Client
{
public:
    Client() {}
    void Start(uint16_t port);
    void Update();
    void handleMessage(Protocol::Message& msg);
    void SendMsg(const void* data, size_t size);
    void Stop();

    ENetHost* client;
    ENetPeer* peer;

    uint32_t myPlayerId;

    //contains allocated pointers, need to manually deallocate them later.
  Game::SpaceShip* ship = nullptr; 

};