#include "config.h"
#include "client.h"
#include "protocol.h"


void Client::Start(uint16_t port)
{


    if (enet_initialize() != 0)
    {
        fprintf(stderr, "[Client] Failed to initialize ENet.\n");
        exit(EXIT_FAILURE);
    }

    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr)
    {
        fprintf(stderr, "[Client] Failed to create ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1"); // Localhost
    address.port = port;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr)
    {
        fprintf(stderr, "[Client] No available peers to connect.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("[Client] sucessfully Connected to the server!\n");;
    }
    else
    {
        enet_peer_reset(peer);
        printf("[Client] Connection failed.\n");
        enet_host_destroy(client);
        client = nullptr;
    }
}

void Client::Update()
{
    ENetEvent event;
    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            printf("[Client] recieved message and handling it.\n");
            Protocol::Message msg = Protocol::DecodeMessage(event.packet);
            handleMessage(msg);
            enet_packet_destroy(event.packet);
            break;
        }

        }
    }
}

void Client::handleMessage(Protocol::Message& msg)
{
    switch (msg.type)
    {
    case Protocol::SpawnPlayer:
    {
        auto* data = static_cast<Protocol::SpawnPlayerData*>(msg.data);
        printf("[Client] Spawn player with ID: %u at (%f, %f, %f)\n",
            data->id, data->Position.x, data->Position.y, data->Position.z);

        //create ship
        ship = new Game::SpaceShip();
        ship->model = Render::LoadModel("assets/space/spaceship.glb");
        ship->position = data->Position;
        break;
  
    }
    case Protocol::Position:
    {
        auto* data = static_cast<Protocol::PositionData*>(msg.data);

            ship->position = data->Position;
            ship->orientation = glm::quat(data->Rotation);

            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), ship->position);
            glm::mat4 rotationMat = glm::toMat4(ship->orientation);
            ship->transform = translationMat * rotationMat;

        break;
    }
    case Protocol::Join:
    {
        auto* data = static_cast<Protocol::JoinData*>(msg.data);
        myPlayerId = data->id; // set your own ID
        printf("[Client] Received Join confirmation with id: %u\n", myPlayerId);
        break;
    }
    case Protocol::Disconnect:
    {
        delete ship;
        ship = nullptr;
        break;
    }
       
    default:
        printf("[Client] Unknown message type: %d\n", msg.type);
        break;
    };

}

void Client::SendMsg(const void* data, size_t size)
{
    // Create a reliable ENet packet with the data size
    ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

    // Send the packet to peer on channel 0
    enet_peer_send(peer, 0, packet);

    // Flush the host immediately
    enet_host_flush(client);
}

void Client::Stop()
{
    if (peer != nullptr)
    {
        ENetEvent event;
        bool disconnected = false;

        // Send disconnect request
        enet_peer_disconnect(peer, 0);

        // Wait up to 3 seconds for a graceful disconnect
        while (enet_host_service(client, &event, 3000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnection succeeded.");
                disconnected = true;
                break;
            default:
                break;
            }

            if (disconnected) break;
        }
        if (!disconnected)
        {
            enet_peer_reset(peer);
        }
        peer = nullptr;

    }
    if (client != nullptr)
    {
        enet_host_destroy(client);
        client = nullptr;
    }

    enet_deinitialize();
}
