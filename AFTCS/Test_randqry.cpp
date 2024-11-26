/*
    作用：找到社区不能满足公平性条件的查询，对于每个点的最大trussness值
    思路：确定每个锚点所在社区的val值，
*/

#include "Truss.h"

VI compute_ktruss(const Graph& G, int q){
    VI res(G.m + 1);
    std::queue<int> Q;
    for(auto eid : G.adj[q]){
        if(G.prop.trussness[eid] >= G.prop.k){
            res[eid] = 1;
            Q.push(eid);
        }
    }
    while(!Q.empty()){
        auto [u, v] = G.edg[Q.front()];
        Q.pop();
        for(auto uv : {u, v}){
            for(auto eid : G.adj[uv]){
                if(G.prop.trussness[eid] >= G.prop.k && !res[eid]){
                    res[eid] = 1;
                    Q.push(eid);
                }
            }
        }
    }
    return res;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cerr << "main: 2 arguments!\n";
        exit(1);
    }

    Graph G = read_graph(argv[1]);
    std::ofstream fout(argv[2]);
    int F = 10, b = 10;

    G.prop.trussness = decomposition(G);
    VI tau_v(G.n + 1), sz(G.n + 1), val(G.n + 1);
    for(int i = 1; i <= G.n; i ++){
        for(auto eid : G.adj[i]){
            tau_v[i] = std::max(tau_v[i], G.prop.trussness[eid]);
        }
    }
    int tau_max = *std::max_element(G.prop.trussness.begin(), G.prop.trussness.end());
    VI vis(G.n + 1);

    int cnt = 0;

    for(int j = 1; j <= G.n; j ++){
        if(!vis[j]){
            G.prop.k = tau_v[j];
            const auto &ktruss = compute_ktruss(G, j);
            std::unordered_set<int> set;
            for(int k = 0; k < G.m; k ++){
                if(ktruss[k]){
                    set.insert(G.edg[k].first);
                    set.insert(G.edg[k].second);
                }
            }
            int vt = compute_value(G, ktruss, F);
            for(auto v : set){
                if(tau_v[v] == tau_v[j]){
                    cnt ++;
                    vis[v] = 1;
                    val[v] = vt;
                    sz[v] = set.size();
                }
            }
            std::cout << 100.0 * cnt / G.n << "% " << cnt << "/" << G.n << "\r";
        }
    }
    std::cout << "\n";
    // for(int i = 1; i <= G.n; i ++){
    //     std::cout << sz[i] << " ";
    // }
    for(int i = 1; i <= G.n; i ++){
        if(F * G.A > val[i]){
            fout << i << " " << F << " " << b << "\n";
        }
    }
}