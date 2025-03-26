
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqtt.h>

#define ADDRESS     "tcp://192.168.0.135"  // IP of your Mosquitto broker (Ubuntu VM)
#define CLIENTID    "RaspberryPiSubscriber"
#define TOPIC       "intialTesting"
#define QOS         1
#define TIMEOUT     10000L

// This callback is called when a message is received
void message_handler(void **unused, struct mqtt_response_publish *message) {
    printf("Received message: %.*s on topic: %.*s\n", message->payload_len, (char*)message->payload, message->topic_name_len, message->topic_name);
}

int main() {
    struct mqtt_client client;
    struct mqtt_network network;
    struct mqtt_transport transport;
    int rc = 0;

    // Initialize MQTT client and network structures
    mqtt_network_init(&network);
    mqtt_client_init(&client, &network, &transport);

    // Connect to the broker
    printf("Connecting to broker %s...\n", ADDRESS);
    rc = mqtt_network_connect(&network, ADDRESS);
    if (rc != 0) {
        printf("Failed to connect to the broker. Error code: %d\n", rc);
        return -1;
    }

    // Setup the client ID and other configurations
    rc = mqtt_client_connect(&client, CLIENTID, NULL, NULL, 0, 60);
    if (rc != 0) {
        printf("Failed to connect to MQTT broker. Error code: %d\n", rc);
        return -1;
    }

    printf("Connected to broker!\n");

    // Set up message callback handler
    mqtt_client_set_message_handler(&client, message_handler);

    // Subscribe to a topic
    printf("Subscribing to topic: %s\n", TOPIC);
    rc = mqtt_subscribe(&client, TOPIC, QOS);
    if (rc != 0) {
        printf("Failed to subscribe to topic. Error code: %d\n", rc);
        return -1;
    }

    // Keep listening for messages
    while (1) {
        rc = mqtt_client_yield(&client, 1000);
        if (rc != 0) {
            printf("MQTT client yield failed. Error code: %d\n", rc);
            break;
        }
    }

    // Clean up
    mqtt_client_disconnect(&client);
    mqtt_network_disconnect(&network);
    return 0;
}