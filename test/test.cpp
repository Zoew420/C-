#include "test.h"
vector<pair<string, void(*)()>> test_cases;

using namespace std;

int main() {
    int cur = 1;
    int n_succeeded = 0;
    
    for (auto& p : test_cases) {
        auto& test = p.first;
        cout << "[" << cur << "/" << test_cases.size() << "] " << test << " : ";
        void (*fp)() = p.second;
        try {
            fp();
            cout << "Passed" << endl;
            n_succeeded++;
        }
        catch (AssertionError & ex) {
            cout << "Assertion failed! " << ex.what() << endl;
        }
        catch (exception & ex) {
            cout << "Unhandled error! " << ex.what() << endl;
        }
        cur++;
    }
    return n_succeeded != test_cases.size();
}