#include "Truss.h"

void solve(Graph& G, int q, int F, int b){
    clock_t beg = clock(), end;
    G.prop.trussness = decomposition(G);
    int k = 0;
    for(auto eid : G.adj[q]){
        k = std::max(k, G.prop.trussness[eid]);
    }
    G.prop.k = k;
    G.prop = compute_property(G, q, F);
    G.prop.layer = compute_layer(G, q, F);
}

#include <bits/stdc++.h>
std::mt19937 rng(std::chrono::system_clock().now().time_since_epoch().count());

Graph rand_graph(){
    Graph G;
    G.n = 100, G.m = 1000;
    G.adj.resize(G.n + 1);
    G.attr.resize(G.n + 1);
    G.prop.deg.resize(G.n + 1);
    std::set<std::pair<int, int>> set;
    for(int i = 0; i < G.m; i ++){
        int u, v;
        do{
            u = rng() % G.n + 1;
            v = rng() % G.n + 1;
            if(v < u){
                std::swap(u, v);
            }
        }while(u == v || set.count({u, v}));
        set.insert({u, v});
        G.edg.push_back({u, v});
        G.adj[u].push_back(i);
        G.adj[v].push_back(i);
        G.prop.deg[u] ++;
        G.prop.deg[v] ++;
    }
    for(int i = 1; i <= G.n; i ++){
        G.attr[i] = rng() % 3;
        G.A = std::max(G.A, G.attr[i] + 1);
    }
    return G;
}

int main(){
    int t = 1000000;
    while(t --){
        if(t % 1000 == 0){
            std::cerr << t << "\n";
        }
        auto G = rand_graph();
        // auto G = read_graph("g.out");
        int q = rng() % G.n + 1, F = rng() % 20 + 1;
        solve(G, q, F, 1);
    }
}