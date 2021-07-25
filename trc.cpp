#include <iostream>
#include <fstream>

#include <vector>
#include <set>
#include <algorithm>

#include <chrono>

#include "./lib.h"
// ==================================== END OF SOURCE ==========================================

int main()
{
    ifstream form ("formula.txt");
    
    vector<vector<int>> f = {{}};
    int t;
    while (form >> t)
    {
        if (t == 0)
            f.push_back({});
        else
            f.back().push_back(t);
    }
    
    vector<long long> cnt = total_random_choices(f, true);
    
    int n = last_var(f);
    
    double E = 0;
    for (int i = 0; i < n; i++)
    {
        cout << 1.0*cnt[i]/factorial(n) << " ";
        E += 1.0*cnt[i]/factorial(n);
    }
    
    cout << endl << "Erandom = " << E << endl;
    
}
