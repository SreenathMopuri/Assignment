#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <regex>
#include <chrono>

bool isValidIPv4(const std::string& ip) {
    int dots = 0;
    std::istringstream ss(ip);
    std::string segment;
    while (std::getline(ss, segment, '.')) {
        // if dots are greater IPv4 address is invalid
        if (++dots > 4) return false;
        // if there is no segment or segement count greater than 3, IPv4 is invalid
        if (segment.empty() || segment.size() > 3) return false;
        // checking in each segement has valid digits or not
        for (char c : segment) if (!isdigit(c)) return false;
        int num = std::stoi(segment);
        // IPv4 address numbers should between 1 to 255
        if (num < 0 || num > 255) return false;
    }
    return dots == 4;
}

/*
* Description: Validate IPv6 address
* input: string (IPv6 address)
* output: boolean (true/false)
*/
// A:B:C:D:E:F:G:H
bool isValidIPv6(const std::string& ip) {
    int colons = 0;
    std::istringstream ss(ip);
    std::string segment;
    while (std::getline(ss, segment, ':')) {
        // if colons are greater than 8, IPv6 is invalid
        if (++colons > 8) return false;
        // segment is empty or segment count greater than 4, IPv6 is invalid
        if (segment.empty() || segment.size() > 4) return false;
        // checking each segment contains hexdigit or not
        for (char c : segment) if (!isxdigit(c)) return false;
    }
    return colons == 8;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file("ip_addresses.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    std::string line;
    int totalIPv4 = 0, totalIPv6 = 0, invalidCount = 0;
    std::unordered_set<std::string> uniqueIPv4, uniqueIPv6;

    while (std::getline(file, line)) {
        if (isValidIPv4(line)) {
            totalIPv4++;
            uniqueIPv4.insert(line);
        }
        else if (isValidIPv6(line)) {
            totalIPv6++;
            uniqueIPv6.insert(line);
        }
        else {
            invalidCount++;
        }
    }

    file.close();

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration by subtracting the start time from the end time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // Output the time taken in microseconds
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

    std::cout << "Total IPv4 address count: " << totalIPv4 << std::endl;
    std::cout << "Total IPv6 address count: " << totalIPv6 << std::endl;
    std::cout << "Unique IPv4 address count: " << uniqueIPv4.size() << std::endl;
    std::cout << "Unique IPv6 address count: " << uniqueIPv6.size() << std::endl;
    std::cout << "Invalid IP address count: " << invalidCount << std::endl;

    return 0;
}
