#include "socket.hh"
#include "util.hh"

#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace std;

void get_URL(const string &host, const string &path) {
    auto address = Address(host, "http");
    auto tcp_socket = TCPSocket();
    tcp_socket.connect(address);
    string request = "GET " + path + " HTTP/1.1" + "\r\n"
                    + "Host: " + host + "\r\n"
                    + "Connection: close\r\n" + "\r\n";
    tcp_socket.write(request);

    while (!tcp_socket.eof()) {
        printf("%s", tcp_socket.read().c_str());
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 0) {
            abort();  // For sticklers: don't try to access argv[0] if argc <= 0.
        }

        // The program takes two command-line arguments: the hostname and "path" part of the URL.
        // Print the usage message unless there are these two arguments (plus the program name
        // itself, so arg count = 3 in total).
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " HOST PATH\n";
            cerr << "\tExample: " << argv[0] << " stanford.edu /class/cs144\n";
            return EXIT_FAILURE;
        }

        // Get the command-line arguments.
        const string host = argv[1];
        const string path = argv[2];

        // Call the student-written function.
        get_URL(host, path);
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
