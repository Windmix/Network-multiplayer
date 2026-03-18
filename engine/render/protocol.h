#pragma once
#include "enet/enet.h"
#include "config.h"

namespace Protocol {
	using namespace glm;

	enum MessageType : char {
		Join,
		Disconnect,
		ClientInput,
		Position,
		PlayerState,
		SpawnPlayer,
		SpawnProjectile,
		SpawnAsteroid,
		StartGame,
	};

	struct Message {
		MessageType type;
		unsigned timeStamp;
		void* data; // Pointer to data struct
	};

	struct JoinData {
		uint32_t id; 
	};
	struct DisconnectData {
		uint32_t id;
	};

	struct PositionData {
		uint32_t id;
		vec3 Position, Velocity, Acceleration;
		vec4 Rotation;
	};

	struct PlayerStateData {
		uint32_t id;
		bool Dead;
		bool Respawn;
	};

	struct SpawnPlayerData {
		uint32_t id;
		vec3 Position;
		vec4 Rotation;
	};

	struct SpawnProjectileData {
		vec3 position;
		vec3 direction;
		float velocity;
	};

	struct ClientInputData {
		uint32_t id;
		bool Forward : 1, Left : 1, Right : 1, RollLeft : 1;
		bool RollRight : 1, PitchUp : 1, PitchDown : 1, Shoot : 1;
	};

	struct SpawnAsteroidData {
		vec3 Position;
		vec4 Rotation;
		uint32_t modelId;
		uint32_t physicsId;
	};

	inline ENetPacket* CreateENetPacket(const Protocol::Message& msg, uint32 enet_flag = ENET_PACKET_FLAG_RELIABLE) {
		uint32_t size = 0;
		switch (msg.type) {
			case Protocol::Join: size = sizeof(JoinData);
				break;
			case Protocol::Disconnect: size = sizeof(DisconnectData); 
				break;
			case Protocol::ClientInput: size = sizeof(ClientInputData);
				break;
			case Protocol::Position: size = sizeof(PositionData);
				break;
			case Protocol::PlayerState: size = sizeof(PlayerStateData); 
				break;
			case Protocol::SpawnPlayer: size = sizeof(SpawnPlayerData); 
				break;
			case SpawnProjectile: size = sizeof(SpawnProjectileData); 
				break;
			case Protocol::SpawnAsteroid: size = sizeof(SpawnAsteroidData);
				break;
			case Protocol::StartGame: 
				break;
			default: assert(false);
		}

		ENetPacket* packet = enet_packet_create(nullptr, sizeof(Message) + size, enet_flag);
		memcpy(packet->data, &msg, sizeof(Message));
		memcpy(packet->data + sizeof(Message), msg.data, size);
		return packet;
	}

	template<typename T>
	inline Protocol::Message CreateMessage(T& messageData, Protocol::MessageType type) {
		Protocol::Message msg;
		msg.type = type;
		msg.data = (void*)&messageData;
		return msg;
	}

	inline Protocol::Message DecodeMessage(ENetPacket* packet) {
		Protocol::Message msg;
		msg.type = ((Message*)packet->data)->type;
		msg.timeStamp = ((Message*)packet->data)->timeStamp;
		msg.data = ((Message*)packet->data) + 1;
		return msg;
	}
}
