#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <chrono>  // For high-resolution clock

// creating mutex object
std::mutex mtx;

// Initializing Global variable count for ipv4, ipv6 and invalidIP
long long int totalIPv4Count = 0, totalIPv6Count = 0, invalidIPCount = 0;

// Initializing uniqueIPv4 and IPv6 Count uisng unordered_set
std::unordered_set<std::string> uniqueIPv4;
std::unordered_set<std::string> uniqueIPv6;

/*
* Description: Validate IPv4 address
* input: string (IPV4 address)
* output: boolean (true/false)
*/
// A.B.C.D 
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

void processLines(const std::vector<std::string>& lines) {
    int localIPv4 = 0, localIPv6 = 0, localInvalid = 0;
    std::unordered_set<std::string> localUniqueIPv4, localUniqueIPv6;

    for (const auto& line : lines) {
        if (isValidIPv4(line)) {
            ++localIPv4;
            localUniqueIPv4.insert(line);
        }
        else if (isValidIPv6(line)) {
            ++localIPv6;
            localUniqueIPv6.insert(line);
        }
        else {
            ++localInvalid;
        }
    }

    // Lock and update global counters
    std::lock_guard<std::mutex> lock(mtx);
    totalIPv4Count += localIPv4;
    totalIPv6Count += localIPv6;
    invalidIPCount += localInvalid;
    uniqueIPv4.insert(localUniqueIPv4.begin(), localUniqueIPv4.end());
    uniqueIPv6.insert(localUniqueIPv6.begin(), localUniqueIPv6.end());
}

int main() {

    // Get the start time
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file("ip_addresses.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;

    // Read file lines into vector
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    int numThreads = std::thread::hardware_concurrency(); // Get available threads
    int chunkSize = lines.size() / numThreads;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startIdx = i * chunkSize;
        int endIdx = (i == numThreads - 1) ? lines.size() : (i + 1) * chunkSize;
        std::vector<std::string> chunk(lines.begin() + startIdx, lines.begin() + endIdx);
        threads.push_back(std::thread(processLines, chunk));
    }

    // Join threads
    for (auto& th : threads) {
        th.join();
    }

    // Get the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration by subtracting the start time from the end time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output the time taken in microseconds
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;


    // Output results
    std::cout << "Total IPv4 address count: " << totalIPv4Count << '\n';
    std::cout << "Total IPv6 address count: " << totalIPv6Count << '\n';
    std::cout << "Unique IPv4 address count: " << uniqueIPv4.size() << '\n';
    std::cout << "Unique IPv6 address count: " << uniqueIPv6.size() << '\n';
    std::cout << "Invalid IP address count: " << invalidIPCount << '\n';

    return 0;
}
