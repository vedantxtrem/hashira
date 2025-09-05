#include <bits/stdc++.h>
using namespace std;

// Convert number string in base "b" to decimal (supports bases up to 36)
long long toDecimal(const string &val, int base) {
    long long result = 0;
    for (char c : val) {
        int digit;
        if (isdigit(c)) digit = c - '0';
        else if (isalpha(c)) digit = tolower(c) - 'a' + 10;
        else continue;
        result = result * base + digit;
    }
    return result;
}

// Lagrange interpolation at x=0
long long lagrangeAtZero(const vector<pair<long long,long long>> &points, int k) {
    long double res = 0.0;
    for (int i = 0; i < k; i++) {
        long double xi = points[i].first;
        long double yi = points[i].second;

        long double li = 1.0;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            long double xj = points[j].first;
            li *= (-xj) / (xi - xj);
        }
        res += yi * li;
    }
    return llround(res); // round to nearest integer
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read entire JSON input from stdin
    string jsonInput((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());

    // Extract n, k
    regex nkRegex(R"REGEX("keys"\s*:\s*\{\s*"n"\s*:\s*(\d+),\s*"k"\s*:\s*(\d+))REGEX");
    smatch nkMatch;
    if (!regex_search(jsonInput, nkMatch, nkRegex)) {
        cerr << "Invalid JSON: missing keys" << endl;
        return 1;
    }
    int n = stoi(nkMatch[1]);
    int k = stoi(nkMatch[2]);

    // Extract all roots
    regex entryRegex(R"REGEX("(\d+)"\s*:\s*\{\s*"base"\s*:\s*"(\d+)",\s*"value"\s*:\s*"([0-9a-zA-Z]+)")REGEX");
    smatch match;
    string::const_iterator searchStart(jsonInput.cbegin());

    map<int, pair<int,string>> inputs;
    while (regex_search(searchStart, jsonInput.cend(), match, entryRegex)) {
        int x = stoi(match[1]);       // the share index
        int base = stoi(match[2]);    // the base
        string val = match[3];        // the encoded value
        inputs[x] = {base, val};
        searchStart = match.suffix().first;
    }

    // Convert to (x, y) points
    vector<pair<long long,long long>> points;
    for (auto &kv : inputs) {
        long long x = kv.first;
        long long y = toDecimal(kv.second.second, kv.second.first);
        points.push_back({x, y});
    }

    // Use only the first k points
    if ((int)points.size() < k) {
        cerr << "Not enough points to reconstruct secret!" << endl;
        return 1;
    }

    long long secret = lagrangeAtZero(points, k);

    cout << "decrypted key :"  << secret << endl;
    
    return 0;
}
