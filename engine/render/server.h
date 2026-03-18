#pragma once
#include <enet/enet.h>
#include <unordered_map>
#include "protocol.h"
#include <thread>
#include "client.h"


class Server
{
private:
	void HandleMessage(ENetPeer* peer, Protocol::Message& msg);
	void BroadcastMessage(ENetPacket* packet);
public:
	std::unordered_map<uint32_t, ENetPeer*> connectedPlayers; // playerId -> peer
	std::atomic<uint32_t> nextPlayerId = 0;
	std::thread T;

	Server() {}

	void Start(uint16_t port);
	void Update();
	void Stop();

	

	void SendSpawnToClients(uint32_t id, glm::vec3 pos, glm::vec4& rot);
	void SendJoinToClient(uint32_t id, ENetPeer* peer);

	ENetHost* server;
	std::unordered_map<uint32_t, Client> ClientShips;
	
};


