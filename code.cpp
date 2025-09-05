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

// Lagrange interpolation - reconstruct polynomial coefficients
vector<long double> lagrangeCoefficients(const vector<pair<long long,long long>> &points, int k) {
    vector<long double> poly(k, 0.0);

    for (int i = 0; i < k; i++) {
        long double xi = points[i].first;
        long double yi = points[i].second;

        // Start with basis polynomial = [1]
        vector<long double> basis = {1.0};
        long double denom = 1.0;

        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            long double xj = points[j].first;

            // Multiply basis by (x - xj)
            vector<long double> newBasis(basis.size() + 1, 0.0);
            for (int b = 0; b < (int)basis.size(); b++) {
                newBasis[b]     -= basis[b] * xj;
                newBasis[b + 1] += basis[b];
            }
            basis = newBasis;

            denom *= (xi - xj);
        }

        long double coeff = yi / denom;
        for (int b = 0; b < (int)basis.size(); b++) {
            poly[b] += coeff * basis[b];
        }
    }

    return poly;
}

// Evaluate polynomial at x
long long evalPoly(const vector<long double> &poly, long long x) {
    long double res = 0.0;
    long double power = 1.0;
    for (auto c : poly) {
        res += c * power;
        power *= x;
    }
    return llround(res);
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

    if ((int)points.size() < k) {
        cerr << "Not enough points to reconstruct secret!" << endl;
        return 1;
    }

    // Build polynomial from first k points
    vector<pair<long long,long long>> chosen(points.begin(), points.begin() + k);
    vector<long double> poly = lagrangeCoefficients(chosen, k);

    // The secret is f(0)
    long long secret = evalPoly(poly, 0);

    cout << "Decrypted key: " << secret << "\n";

    // Check all points for correctness
    cout << "Wrong shares:\n";
    bool foundWrong = false;
    for (auto &p : points) {
        long long expected = evalPoly(poly, p.first);
        if (expected != p.second) {
            cout << "  Share x=" << p.first << " is wrong (got " << p.second << ", expected " << expected << ")\n";
            foundWrong = true;
        }
    }
    if (!foundWrong) {
        cout << "  None\n";
    }

    return 0;
}
