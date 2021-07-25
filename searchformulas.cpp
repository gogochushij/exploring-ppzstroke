#include <iostream>
#include <fstream>

#include <vector>
#include <set>
#include <algorithm>

#include <chrono>

#include "./lib.h"

int main()
{
    ofstream records;
    
    int n;
    cout << "NUMBER OF VARIABLES: ";
    cin >> n;
    int seed;
    cout << "SEED: ";
    cin >> seed;
    
    long long ideal = factorial(n)*n;
    long long formulas_checked = 0;
    set< pair<long long, vector<vector<int>>>, decltype(&cmp_records)> Records (cmp_records);
    
    chrono::steady_clock::time_point cbegin = chrono::steady_clock::now();
    while (true)
    {
        vector<vector<int>> f = generate_singlesat_formula(n);
        sort(f.begin(), f.end(), cmp_clause);

        vector<long long> cnt = total_random_choices(f);

        
        long long sum = 0;
        for (long long c : cnt) sum += c;
        
        //cout << sum << " (" << 100.0*sum/ideal << "%)" << endl;
        
        long long celapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - cbegin).count();
        if (celapsed >= 5)
        {
            records.open("records.txt");
            for (pair< long long, vector<vector<int>> > p : Records)
            {
                records << p.first << " (" << 100.0*p.first/ideal << "%)" << endl;
                for (vector<int> cl : p.second)
                {
                    for (int lit : cl)
                    {
                        records << lit << " ";
                    }
                    records << "0" << endl;
                }
                records << "------------------" << endl;
            }
            records.close();
            cbegin = chrono::steady_clock::now();
            
            cout << formulas_checked << " formulas checked" << endl;
        }
        
        
        Records.insert({sum, f});
        while (Records.size() >= 10) Records.erase(--Records.end());
        formulas_checked++;
    }
    
}
