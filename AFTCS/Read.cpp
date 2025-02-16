#include "Truss.h"

Graph read_graph(std::string file){
    Graph G;
    std::ifstream fin(file);
    fin >> G.n >> G.m;
    G.adj.resize(G.n + 1);
    G.attr.resize(G.n + 1);
    G.prop.deg.resize(G.n + 1);
    for(int i = 0; i < G.m; i ++){
        int u, v;
        fin >> u >> v;
        G.edg.push_back({u, v});
        G.adj[u].push_back(i);
        G.adj[v].push_back(i);
        G.prop.deg[u] ++;
        G.prop.deg[v] ++;
    }
    for(int i = 1; i <= G.n; i ++){
        fin >> G.attr[i];
        G.A = std::max(G.A, G.attr[i] + 1);
    }
    // G.A = -1;
    return G;
}

std::vector<std::tuple<int, int, int, int>> read_query(std::string file){
    std::ifstream fin(file);
    int q, F, R, b;
    std::vector<std::tuple<int, int, int, int>> query;
    while(fin >> q >> F >> R >> b){
        query.push_back({q, F, R, b});
    }
    return query;
}