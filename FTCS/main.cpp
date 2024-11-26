#pragma GCC optimize(2,"Ofast")
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math")
// #pragma GCC target("sse,sse2,sse3,ssse3,sse4.1,sse4.2,avx,avx2,popcnt,tune=native")

#include <bits/stdc++.h>
#include "utils.hpp"
#include "truss.hpp"
#define debug(a) cout << (#a) << " : " << (a) << endl
using namespace std;
bool in_que[maxm];
Graph FTCS() {
    cerr << "FTCS running...\n";
    clock_t start = clock();

    Graph C, ret;
    Graph tmp;

    ret.query_dis = 0x3f3f3f3f;
    int lk = 2, rk = tau[q] + 1, k;
    while(lk < rk) {
    // for (int i = 2;i <= tau[q];i++) {
        k = (lk + rk) >> 1;
        // k = i;
        tmp.V.clear();
        memset(vis, false, sizeof(vis));

        queue<int> que;
        que.push(q);
        vis[q] = true;
        tmp.V.push_back(q);

        while(!que.empty()) {
            int u = que.front();
            que.pop();
            for(const int & eid : G[u]) {
                int v = E[eid].u ^ E[eid].v ^ u;
                if(!vis[v] && E[eid].trussness >= k) {
                    vis[v] = true;
                    tmp.V.push_back(v);
                    que.push(v);
                }
            }
        }
        memset(attr_sum, 0, sizeof(attr_sum));
        for(const int v : tmp.V) {
            attr_sum[phi[v]]++;
        }
        int fairness = 0;
        for(const int attr : attr_set) {
            fairness += min(F, attr_sum[attr]);
        }
        cerr << "lk : " << lk << " , rk : " << rk << ", fairness:" << fairness << '\n';
        if(fairness == F * (int) attr_set.size()) {
            lk = k + 1;
            C.V.clear();
            for(const int x : tmp.V) C.V.push_back(x);
        } else {
            rk = k;
            // k--;
            // break;
        }
        // cerr << "fairness:" << F * (int) attr_set.size() << '\n';
    }
    k = lk - 1;
    debug(k);
    if(k <= 2) {
        cerr << "No solotion!\n";
        assert(0);
    }

    memset(vis, false, sizeof(vis));
    queue<int> que; // 把边加进去
    que.push(q);
    vis[q] = true;

    while(!que.empty()) {
        int u = que.front();
        que.pop();
        for(const int & eid : G[u]) {
            int v = E[eid].u ^ E[eid].v ^ u;
            if(!vis[v] && E[eid].trussness >= k) {
                vis[v] = true;
                C.E.insert(hash_table_2[1ll * v * n + u]);
                C.G[u].insert(v);
                C.G[v].insert(u);
                que.push(v);
            }
        }
    }
    // ----------------------------------------------------------------------------------

    auto cmp = [&C](const int &v1, const int &v2) -> bool
    {
        return C.G[v1].size() != C.G[v2].size() ? C.G[v1].size() > C.G[v2].size() : v1 < v2;
    };
    sort(C.V.begin(), C.V.end(), cmp);
    cerr << "C.V size: " << C.V.size() << '\n';
    if(C.V.size() >= 10000) {
        assert(0);
    }
    unordered_set<int> A[maxn];

    for (int i = 0; i < C.V.size(); i++)
    {
        int vertex = C.V[i];
        for (const int v : C.G[vertex])
        {
            if (cmp(v, vertex))
                continue;
            for (const int &w : A[vertex])
            {
                if (A[v].count(w))
                {
                    int eid1 = hash_table_2[1ll * v * n + vertex], eid2 = hash_table_2[1ll * v * n + w], eid3 = hash_table_2[1ll * vertex * n + w];
                    C.trussness[eid1]++;
                    C.trussness[eid2]++;
                    C.trussness[eid3]++;
                    C.triangles[eid1].insert({min(eid2, eid3), max(eid2, eid3)});
                    C.triangles[eid2].insert({min(eid1, eid3), max(eid1, eid3)});
                    C.triangles[eid3].insert({min(eid1, eid2), max(eid1, eid2)});
                }
            }
            A[v].insert(vertex);
        }
    }

    // for(const int & eid : C.E) {
    //     cerr << E[eid].u << ", " << E[eid].v << " triangle size:" << C.triangles[eid].size() << '\n';
    // }

    // // -----------------------------------------------------------------------------
    
    // clock_t start = clock();
    clock_t end = clock();
    cerr << "Binary search running time: " << double(end - start) / CLOCKS_PER_SEC << "s." << endl;
    clock_t t_dist = 0, t_maintain = 0;
    int tot = 0;
    while(1) {
        clock_t t1 = clock();
        queue<int> que;
        que.push(q);
        memset(dis, 0x3f, sizeof(dis));
        dis[q] = 0;
        while(!que.empty()) { // 计算出每个点到查询点的距离
            int u = que.front();
            que.pop();
            for(const int v : C.G[u]) {
                if(dis[v] == 0x3f3f3f3f) {
                    dis[v] = dis[u] + 1;
                    // cerr << v << ": " << dis[v] << endl;
                    que.push(v);
                }
            }
        }
        int attr_sum2[maxa + 5];
        memset(attr_sum2, 0, sizeof(attr_sum2));
        for(const int v : C.V) {
            attr_sum2[phi[v]]++;
        }
        sort(C.V.begin(), C.V.end(), [&attr_sum2] (const int &v1, const int &v2) -> bool {
            return (dis[v1] != dis[v2]) ?  (dis[v1] > dis[v2]) : (attr_sum2[phi[v1]] > attr_sum2[phi[v2]]);
        });

        clock_t t2 = clock();
        t_dist += t2 - t1;
        // ofstream fout("./case.txt");
        // cerr <<(int) C.V.size() << endl;
        // sort(C.V.begin(), C.V.end());
        // for(const int u : C.V) {
        //     cerr << u << " ";
        // }
        // cerr << endl;

        assert(que.empty());

        C.query_dis = dis[C.V[0]];
        tot++;
        if(tot % 10 == 0) cerr << "iter : " << tot << ", current query_dis: " << ret.query_dis << ", C.V.size:" << C.V.size() << '\n';
        
        if(C.query_dis < ret.query_dis) {
            ret.V.clear();
            ret.G.clear();
            for(const int x : C.V) {
                ret.V.emplace_back(x);
                for(const int y : C.G[x]) {
                    ret.G[x].insert(y);
                }
            }
            ret.query_dis = C.query_dis;
            cerr << "iter : " << tot << ", current query_dis: " << ret.query_dis << ", C.V.size:" << C.V.size() << '\n';
        }

        memset(in_que, false, sizeof(in_que));

        for(int i = 0;i < C.V.size() && i < _gamma;i++) {
            int u_star = C.V[i];
            // if(i == 0) cerr << "u* : " << u_star << " " << C.V.size() << '\n';
            for(const int u : C.G[u_star]) {
                int eid = hash_table_2[1ll * u_star * n + u];
                if(!in_que[eid]) {
                    in_que[eid] = true;
                    que.push(eid);
                }
            }
        }

        clock_t t3 = clock();
        while(!que.empty()) {
            int eid1 = que.front();
            // if(C.V.size() == ) debug(E[eid1].u);
            // debug(E[eid1].v);
            que.pop();
            int u = E[eid1].u, v = E[eid1].v;
            C.G[u].erase(v), C.G[v].erase(u);
            C.E.erase(eid1);

            for(auto ele : C.triangles[eid1]) {
                int eid2 = ele.first, eid3 = ele.second;
                // debug(E[eid2].u);
                // debug(E[eid2].v);

                C.trussness[eid2]--;
                C.trussness[eid3]--;
                
                C.triangles[eid2].erase({min(eid1, eid3), max(eid1, eid3)});
                if(C.trussness[eid2] < k - 2 && !in_que[eid2]) {
                    in_que[eid2] = true;
                    que.push(eid2);
                }
                C.triangles[eid3].erase({min(eid1, eid2), max(eid1, eid2)});
                if(C.trussness[eid3] < k - 2 && !in_que[eid3]) {
                    in_que[eid3] = true;
                    que.push(eid3);
                }
            }
            C.triangles[eid1].clear();
        }
        clock_t t4 = clock();
        t_maintain += t4 - t3;
        sort(C.V.begin(), C.V.end(), [&C] (const int &v1, const int &v2) -> bool {
            return C.G[v1].size() > C.G[v2].size();
        });

        while((int) C.V.size() > 0 && C.G[C.V[C.V.size() - 1]].size() == 0) {
            C.V.pop_back();
        }


        // for(const int u : C.V) {
        //     cerr << u << " ";
            // assert((int) C.G[u].size() <= k - 2);
        // }
        // cerr << endl;

        memset(attr_sum, 0, sizeof(attr_sum));
        for(const int v : C.V) {
            attr_sum[phi[v]]++;
        }
        int fairness = 0;
        for(const int attr : attr_set) {
            fairness += min(F, attr_sum[attr]);
        }
        // cerr << fairness << endl;
        if(fairness < F * (int) attr_set.size()) break;
    }

    cerr << "Distance time: " << double(t_dist) / CLOCKS_PER_SEC << "s.\n";
    cerr << "Maintain time: " << double(t_maintain) / CLOCKS_PER_SEC << "s.\n";

    clock_t endend = clock();
    cerr << "Delete running time: " << double(endend - end) / CLOCKS_PER_SEC << "s.\n";
    cout << "Overall running time: " << double(endend - start) / CLOCKS_PER_SEC << "s.\n";
    return ret;
}

mt19937 rnd(time(0));
int get_rand(int l, int r) {
    return l + rnd() % (r - l + 1);
}

int main(int argc, char * argv[])
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    if(argc != 6) {
        cerr << "Usage: ./main <dataset> <F> <q> <attr_range> <gamma>\n";
        return -1;
    }

    string dataset(argv[1]);
    cerr << "Loading ../Dataset/"+ dataset + "/" + dataset + ".txt" << endl;
    FILE *result = freopen(("../Dataset/"+ dataset + "/" + dataset + ".txt").c_str(), "r", stdin);
    if (result == NULL) {
        cerr << "No such file!\n";
        return -1;
    }


    F = stoi(argv[2]);
    q = stoi(argv[3]);
    int attr_range = stoi(argv[4]);
    _gamma = stoi(argv[5]);

    freopen(("./output/dataset_" + dataset + "_F_" + to_string(F) + "_q_" + to_string(q) + "_R_" + to_string(attr_range) + "_gamma_" + to_string(_gamma) + ".txt").c_str(), "w", stdout);

    // if(!data_info.count(dataset)) {
    //     cerr << "Wrong dataset!\n"; 
    //     assert(0);
    // }

    // n = data_info[dataset].first;
    // m = data_info[dataset].second;
    cin >> n >> m;

    for (int i = 1; i <= m; i++)
    {
        cin >> E[i].u >> E[i].v;
        if(i % 200000 == 0) cerr << "Reading edge " << fixed << setprecision(4) << 100.0 * i / m << "%.\n";
        if(E[i].u == E[i].v)
            cerr << "Self loop!\n";
        assert(E[i].u != E[i].v);
        if(hash_table_1.count(1ll * E[i].u * n + E[i].v)) continue;
        if(hash_table_1.count(1ll * E[i].v * n + E[i].u)) continue;
        hash_table_1[1ll * E[i].u * n + E[i].v] = i;
        hash_table_1[1ll * E[i].v * n + E[i].u] = i;
        hash_table_2[1ll * E[i].u * n + E[i].v] = i;
        hash_table_2[1ll * E[i].v * n + E[i].u] = i;
        D[E[i].u]++;
        D[E[i].v]++;
        G[E[i].u].push_back(i);
        G[E[i].v].push_back(i);

        e_link.insert(E[i].u, E[i].v, i);
        e_link.insert(E[i].v, E[i].u, i);
    }

    cerr << "Reading finished." << endl;


    // result = freopen(("../Dataset/"+ dataset + "/att.txt").c_str(), "r", stdin);
    // if (result == NULL) {
    //     cerr << "No such file!\n";
    //     return -1;
    // }
    for(int i = 1;i <= n;i++) { 
        // int xx;cin >> xx;
        // cin >> phi[i];
        phi[i] = get_rand(0, attr_range - 1);
        attr_set.insert(phi[i]);
    }
    cerr << "Truss decomposition running...\n";
    truss_decomposition();
    cerr << "Truss decomposition finished." << endl;

    Graph ans = FTCS();
    cout << "diameter : " << compute_diam(ans) << '\n';
    return 0;
}