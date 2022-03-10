#include <string>
#include <algorithm>
#include <fmt/printf.h>

#include <socks/socks.hpp>

int main(int argc, char *argv[])
{
    std::string motd = "ECHO Daemon v1.2 [SKS Edition]\r\n";
    std::vector<uint8_t> motd_message = std::vector<uint8_t>(motd.begin(), motd.end());

    // Create the master socket, handles all incoming connections
    sks::socket master_socket = sks::socket(sks::IPv4, sks::stream);

    // Generate the address parameters for the master socket
    sks::address host = sks::address("localhost:8888");

    master_socket.socketOption(sks::reuseAddr, true, sks::socketLevel);

    // Attempt to bind the address parameter to the master socket, catch if it fails
    try {
        master_socket.bind(host);
    }
    catch (std::exception& e) {
        fmt::print(stderr, "Bind Error: \n{}\n", e.what());
    }

    fmt::print("Listener on address {}\n", master_socket.localAddress().name());

    // Try to specify maximum of 3 pending connections for the master socket
    try {
        master_socket.listen(3);
    }
    catch (std::exception& e) {
        fmt::print(stderr, "Listen failure: \n{}\n", e.what());
    }

    fmt::print("Waiting for connections...\n");

    std::vector<sks::socket> clientList;

    while (true)
    {
        // Try to check for new socket connection, and add it to the clientList vector
        try {
            // Is master socket ready to receive a new client socket?
            if (master_socket.readReady())
            {
                // Push new client socket to the back of the clientList vector
                clientList.push_back(master_socket.accept());

                // Get a reference to the last client in the clientList (which will be the last one pushed back)
                sks::socket& lastConnectedClient = clientList.back();
                // Convert to sks::IPv4Address so we can easily pull the IP and Port
                sks::IPv4Address info = (sks::IPv4Address)lastConnectedClient.connectedAddress();

                fmt::print("New client connect. Address is {}\n", info.name());
                // Send the motd as a vector of bytes (uint8_t)
                lastConnectedClient.send(motd_message);
            }
        }
        catch (std::exception& e) {
            fmt::print(stderr, "Reading data on master_socket FAILED\n{}\n", e.what());
        }

        for (auto& l : clientList)
        {
            try {
                if (l.readReady())
                {
                    std::vector<uint8_t> bytes = l.receive();

                    if (bytes.empty())
                    {
                        fmt::print("{} has disconnected\n", l.connectedAddress().name());
                        clientList.erase(std::remove(clientList.begin(), clientList.end(), l), clientList.end());
                        continue;
                    }

                    fmt::print("{}> {}", l.connectedAddress().name(), std::string(bytes.begin(), bytes.end()));
                    for (auto& cl : clientList)
                    {
                        if (l != cl)
                        {
                            cl.send(bytes);
                        }
                    }
                }
            }
            catch (std::exception& e)
            {
                fmt::print(stderr, "Echo error:\n{}\n", e.what());
            }
        }
    }
    return 0;
}
