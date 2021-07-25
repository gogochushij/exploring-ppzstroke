using namespace std;

int last_var(const vector<vector<int>> &f) // f is a formula
{
    int lv = 0;
    for (vector<int> clause : f)
    {
        for (int literal : clause)
        {
            lv = max(lv, abs(literal));
        }
    }
    
    return lv;
}

bool evaluate_clause(const vector<int> &clause, const vector<char> &asmt)
{
    for (int literal : clause)
    {
        int lit_value = (literal>0 ? asmt[abs(literal)-1] : !asmt[abs(literal)-1]);
        if (lit_value == true)
            return true;
    }
    return false;
}

bool evaluate_cnf(const vector<vector<int>> &f, const vector<char> &asmt) //asmt = assignment
{
    for (vector<int> clause : f)
    {
        if (evaluate_clause(clause, asmt) == false)
            return false;
    }
    return true;
}

bool increment_asmt(vector<char> &asmt)
{
    asmt[0]++;
    int j = 0;
    bool overflow = true;
    while (asmt[j] == 2)
    {
        asmt[j] = 0;
        if (j+1 < asmt.size())
            asmt[j+1] += 1;
        else
            overflow = false;
        j++;
    }
    
    return overflow;
}

int num_of_sat_asmt(const vector<vector<int>> &f, int uplim=2)
{
    vector<char> asmt(last_var(f), 0);
    
    int num = 0;
    do
    {
        if (evaluate_cnf(f, asmt) == true)
            num++;
        if (num == uplim and uplim > 0)
            return num;
        
    } while (increment_asmt(asmt));
    
    return num;
}

bool cmp_lit(int a, int b)
{
    return abs(a) < abs(b);
}

vector<int> generate_clause(int n)
{
    vector<int> clause(3);
    for (int j=0; j<3; j++)
    {
        clause[j] = rand()%n + 1;
        for (int i=0; i<j; i++)
        {
            if (clause[i] == clause[j])
            {
                j--;
                break;
            }
        }
    }
    
    int neg = rand()%7;
    clause[0] *= (neg&1 ? -1 : 1);
    clause[1] *= (neg&2 ? -1 : 1);
    clause[2] *= (neg&4 ? -1 : 1);
    sort(clause.begin(), clause.end(), cmp_lit);
    
    return clause;
}

vector<vector<int>> generate_singlesat_formula(int n)
{
    vector<char> unit_vector = vector<char>(n, 1);
    vector<vector<int>> f;
    bool flag;
    do
    {
        vector<int> clause = generate_clause(n);
        
        if (find(f.begin(), f.end(), clause) != f.end())
            flag = true;
        else
        {
            f.push_back(clause);
            flag = (num_of_sat_asmt(f) == 2);
        }
    } while (flag);
    
    return f;
}

///===========================================================================================================
///===========================================================================================================
///===========================================================================================================
///===========================================================================================================
///===========================================================================================================
///===========================================================================================================

bool cmp_clause(vector<int> cl1, vector<int> cl2)
{
    sort(cl1.begin(), cl1.end(), cmp_lit);
    sort(cl2.begin(), cl2.end(), cmp_lit);
    for (int j=0; j<3; j++)
    {
        if (abs(cl1[j]) != abs(cl2[j]))
            return (abs(cl1[j]) < abs(cl2[j]));
    }
    
    int neg1 = (cl1[0]>0)*4 + (cl1[1]>0)*2 + (cl1[2]>0)*1,
        neg2 = (cl2[0]>0)*4 + (cl2[1]>0)*2 + (cl2[2]>0)*1;
    return neg1 < neg2;
}

bool do_unit_clause(vector<vector<int>> &formula)
{
    //bool changes_made = false;
    for (vector<int> unitcl : formula)
    {
        if (unitcl.size() == 1)
        {
            int unitlit = unitcl[0];
            for (vector<vector<int>>::iterator cl_it = formula.begin(); cl_it != formula.end(); cl_it++)
            {
                for (vector<int>::iterator lit_it = cl_it->begin(); lit_it != cl_it->end(); lit_it++)
                {
                    if (*lit_it == unitlit)
                    {
                        formula.erase(cl_it--);
                        //changes_made = true;
                        break;
                    }
                    else if (*lit_it == -unitlit)
                    {
                        //changes_made = true;
                        (*cl_it).erase(lit_it--);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool purge_similar_literals(vector<int> &clause)
{
    for (int i = 0; i < clause.size(); i++)
        for (int j = i+1; j < clause.size(); j++)
            if (abs(clause[i]) == abs(clause[j]))
            {
                //cout << "PURGED ";
                //for (int k : clause) cout << k << " ";
                //cout << endl;
                
                if (clause[i] == clause[j])
                {
                    clause.erase(clause.begin() + i);
                }
                else if (clause[i] == - clause[j])
                {
                    clause = {};
                }
                return true;
            }
    return false;
}

bool purge_similar_literals_whole_formula(vector<vector<int>> &f)
{
    for (int s = 0; s < f.size(); s++)
    {
        while (purge_similar_literals(f[s]));
    }
    
    return false;
}

int do_equivalence(vector<vector<int>> &formula, const vector<int> &perm)
{
    for (int i = 0; i < formula.size(); i++)
    {
        if (formula[i].size() != 2) continue;
        for (int j = i+1; j < formula.size(); j++)
        {
            if (formula[j].size() != 2) continue;
            sort(formula[i].begin(), formula[i].end(), cmp_lit);
            sort(formula[j].begin(), formula[j].end(), cmp_lit);
            if (formula[i][0] == - formula[j][0] and
                formula[i][1] == - formula[j][1] )
            {
                int from_lit = formula[i][0], to_lit = formula[j][1];
                int pos_i = find(perm.begin(), perm.end(), abs(from_lit)) - perm.begin(),
                    pos_j = find(perm.begin(), perm.end(), abs(to_lit)) - perm.begin();
                if (pos_i > pos_j)
                {
                    swap(pos_i, pos_j);
                    swap(from_lit, to_lit);
                }
                
//                for (vector<vector<int>>::iterator cl_it = formula.begin(); cl_it != formula.end(); cl_it++)
//                {
//                    for (vector<int>::iterator lit_it = cl_it->begin(); lit_it != cl_it->end(); lit_it++)
//                    {
//                        if (*lit_it == from_lit)
//                            *lit_it = to_lit;
//                        else if (*lit_it == - from_lit)
//                            *lit_it = - to_lit;
//                    }
//
//                    while (purge_similar_literals(*cl_it));
//                }
                for (int s = 0; s < formula.size(); s++)
                {
                    for (int t = 0; t < formula[s].size(); t++)
                    {
                        if (formula[s][t] == from_lit)
                            formula[s][t] = to_lit;
                        if (formula[s][t] == - from_lit)
                            formula[s][t] = - to_lit;
                    }
                }
                
                return from_lit;
            }
        }
    }
    return false;
}

bool del_empty_clauses(vector<vector<int>> &f)
{
    bool smth_deleted = false;
    for (vector<vector<int>>::iterator it = f.begin(); it != f.end(); it++)
    {
        if (it->empty() == true)
        {
            f.erase(it--);
            smth_deleted = true;
        }
    }
    
    return smth_deleted;
}

void thrash_the_formula(vector<vector<int>> &f, const vector<int> &perm)
{
    while (purge_similar_literals_whole_formula(f) or do_unit_clause(f) or do_equivalence(f, perm) or del_empty_clauses(f));
}

void print_formula(const vector<vector<int>> &f)
{
    for (vector<int> cl : f)
    {
        for (int lit : cl)
        {
            cout << lit << " ";
        }
        cout << 0 << endl;
    }
    cout << string(20, '-') << endl;
}

void read_formula(vector<vector<int>> &f)
{
    ifstream inp("formula.txt");
    
    f = {{}};
    int t;
    while (inp >> t)
    {
        if (t == 0)
        {
            f.push_back({});
        }
        else
        {
            f.back().push_back(t);
        }
    }
    
    del_empty_clauses(f);
}

vector<int> random_choices(vector<vector<int>> f, vector<int> perm)
{
    vector<int> rnd_ch (perm.size(), false);
    
    for (int i = 0; i < perm.size(); i++)
    {
        int var = perm[i];
        
        thrash_the_formula(f, perm);
        
        for (vector<vector<int>>::iterator cl_it = f.begin(); cl_it != f.end(); cl_it++)
        {
            for (vector<int>::iterator lit_it = cl_it->begin(); lit_it != cl_it->end(); lit_it++)
            {
                if (abs(*lit_it) == var)
                {
                    rnd_ch[i] = true;
                    if (*lit_it > 0)
                    {
                        f.erase(cl_it--);
                        break;
                    }
                    else if (*lit_it < 0)
                    {
                        cl_it->erase(lit_it--);
                    }
                }
            }
        }
    }
    
    return rnd_ch;
}

vector<long long> total_random_choices(const vector<vector<int>> &f, bool debug = false)
{
    int lv = last_var(f);
    vector<long long> cnt (lv, 0);
    
    vector<int> perm = {};
    for (int i = 1; i <= lv; i++)
        perm.push_back(i);
    
    do
    {
        vector<int> rd = random_choices(f, perm);
        
        if (debug)
        {
            for (int i : perm) cout << i;
            cout << " ";
            for (int i : rd) cout << i;
            cout << endl;
        }
        
        for (int i=0; i<lv; i++)
            cnt[perm[i]-1] += rd[i];
    } while (next_permutation(perm.begin(), perm.end()));
    
    return cnt;
}

long long factorial(int n)
{
    long long p = 1;
    for (int i = 2; i <= n; i++) p *= i;
    return p;
}

bool cmp_records(pair<long long, vector<vector<int>>> a, pair<long long, vector<vector<int>>> b)
{
    return a.first > b.first;
}

