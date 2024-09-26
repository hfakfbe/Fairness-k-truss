#include "Truss.h"

Graph test_maxk(const Graph& G, int q, int F, int k){
    auto tritruss = compute_triangle_three_truss(G);

    VI res = compute_maximal_ktruss(G, tritruss, k, q);
    Graph G2;
    G2.n = G.n;
    G2.A = G.A;
    G2.adj.resize(G.n + 1);
    G2.attr = G.attr;
    G2.prop.deg.resize(G.n);
    for(int i = 0; i < G.m; i ++){
        if(res[i]){
            auto [u, v] = G.edg[i];
            G2.adj[u].push_back(G2.edg.size());
            G2.adj[v].push_back(G2.edg.size());
            G2.prop.trussness.push_back(G.prop.trussness[i]);
            G2.edg.push_back(G.edg[i]);
            G2.prop.deg[u] ++;
            G2.prop.deg[v] ++;
        }
    }
    G2.m = G2.edg.size();
    G2.prop.k = k;
    G2.prop = update_property(G2, res, q, F);

    return G2;
}

void solve(Graph& G, int q, int F, int b){

    std::cerr << "decomposition\n";
    
    clock_t beg = clock(), end;
    G.prop.trussness = decomposition(G); // ac
    end = clock();
    std::cerr << end - beg << "us\n";
    
    
    // std::cerr << "compute_maxk\n";
    auto G2 = test_maxk(G, q, F, 4);
    for(auto [u, v] : G2.edg){
        std::cout << u << " " << v << "\n";
    }
}

int main(){
    Graph G = read_graph("../testmaxk.txt");

    std::vector<std::tuple<int, int, int>> query = read_query("../testmaxk_qry.txt");

    for(int i = 0; i < query.size(); i ++){
        auto [q, F, b] = query[i];

        clock_t beg = clock(), end;
        solve(G, q, F, b);
        end = clock();
        
        std::cerr << end - beg << "us\n";
    }

}