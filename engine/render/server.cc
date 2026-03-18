#include "config.h" // keep this always on top
#include "server.h"
#include "enet/enet.h"
#include <iostream>
//
void Server::Start(uint16_t port)
{

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    server = enet_host_create(&address, 32, 2, 0, 0); // 32 clients max

    if (server == nullptr)
    {
        fprintf(stderr, "[server] An error occurred while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }
    T = std::thread(&Server::Update, this);
    printf("[Server] server created \n");
}

void Server::Update()
{
    ENetEvent event;
    while (true)
    {
        while (enet_host_service(server, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                uint32_t playerId = nextPlayerId++;
                connectedPlayers[playerId] = event.peer;
                event.peer->data = reinterpret_cast<void*>(playerId);
                std::cout << "[server] a new client has connected ";
                printf("from %x:%u.\n",
                    event.peer->address.host,
                    event.peer->address.port);

                //send join and spawn client
                SendJoinToClient(playerId, event.peer);
                printf("[server] Player joined with ID %d\n", playerId);
                glm::vec3 pos = { 0, 2, 0 };
                glm::vec4 rot = { 0, 0, 0, 1 };
                SendSpawnToClients(playerId, pos, rot);
                break;
            }   
            case ENET_EVENT_TYPE_RECEIVE:
            {

                Protocol::Message msg;
                memcpy(&msg, event.packet->data, sizeof(Protocol::Message));
                HandleMessage(event.peer, msg);
                printf("[Server] Message received. Type: %d\n", msg.type);
                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                uint32_t recievedPlayerID = reinterpret_cast<uint32_t>(event.peer->data);
                std::cout << "[server] Player " << recievedPlayerID << " disconnected.\n";
                connectedPlayers.erase(recievedPlayerID);
                event.peer->data = nullptr;
                printf("[Server] Client disconnected.\n");
                break;
            }
                
            default:
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // avoid burning CPU
    }
}

void Server::Stop()
{
    if (server != nullptr)
    {

        enet_host_destroy(server);
        server = nullptr;
    }
    enet_deinitialize();
}
void Server::SendSpawnToClients(uint32_t id, glm::vec3 pos, glm::vec4& rot)
{
    //Fill in ship spawn data
    Protocol::SpawnPlayerData data;
    data.id = id;
    data.Position = pos;
    data.Rotation = rot;

    //pack and send msg to client
    Protocol::Message msg = Protocol::CreateMessage(data, Protocol::SpawnPlayer);
    ENetPacket* shipPacket = Protocol::CreateENetPacket(msg);

    // Send to all connected players
    BroadcastMessage(shipPacket);
    
    //Clean up the original packet (copies already sent)
    enet_packet_destroy(shipPacket);
    printf("[server] sucessfully informed other clients of a client joined!\n");


}

void Server::SendJoinToClient(uint32_t id, ENetPeer* peer)
{
    Protocol::JoinData data;
    data.id = id;
    Protocol::Message msg = Protocol::CreateMessage(data, Protocol::Join);
    ENetPacket* packet = Protocol::CreateENetPacket(msg);
    enet_peer_send(peer, 0, packet);
}

void Server::HandleMessage(ENetPeer* peer, Protocol::Message& msg)
{
    switch (msg.type)
    {
    case Protocol::MessageType::Join:
    {
        uint32_t playerId = nextPlayerId++;
        //send join and spawn client
        SendJoinToClient(playerId, peer);
        printf("[server] Player joined with ID %d\n", playerId);
        glm::vec3 pos = { 0, 2, 0 };
        glm::vec4 rot = { 0, 0, 0, 1 };
        SendSpawnToClients(playerId, pos, rot);
        break;
        

        break;
    }
    case Protocol::MessageType::ClientInput:
    {
        // Handle input for input->id
        auto* input = static_cast<Protocol::ClientInputData*>(msg.data);
        // Add your input handling logic here
        break;
    }
    case Protocol::MessageType::Disconnect:
    {
        // Find and remove the disconnecting player
        for (auto it = connectedPlayers.begin(); it != connectedPlayers.end(); ++it)
        {
            if (it->second == peer)
            {
                connectedPlayers.erase(it);
                peer->data = nullptr;;
                break;
            }
        }
        break;
    }
    default:
        printf("[server] Unknown message type received.\n");
        break;
    }
}

void Server::BroadcastMessage(ENetPacket* packet)
{
    //For each peer (network connection) in the connectedPlayers map, get its associated player ID.
    for (auto& [id, peer] : connectedPlayers)
    {
        // Make a copy of the packet for each peer (ENet requires this)
        ENetPacket* packetCopy = enet_packet_create(
            packet->data,
            packet->dataLength,
            ENET_PACKET_FLAG_RELIABLE);

        //Send to each peer
        enet_peer_send(peer, 0, packetCopy);
    }
}

