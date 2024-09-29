#include "Truss.h"

int main(){
    Graph G = read_graph("../facebook_A4.txt");
    std::ofstream fout("../facebook_qry.txt");
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
    for(int i = 3; i <= tau_max; i ++){
        G.prop.k = i;
        for(int j = 1; j <= G.n; j ++){
            if(!vis[j] && tau_v[j] == i){
                auto prop = compute_property(G, j, -1);
                std::set<int> set;
                for(int k = 0; k < G.m; k ++){
                    if(prop.ktruss[k]){
                        set.insert(G.edg[k].first);
                        set.insert(G.edg[k].second);
                    }
                }
                int vt = compute_value(G, prop.ktruss, F);
                for(auto v : set){
                    if(tau_v[v] == i){
                        vis[v] = 1;
                        val[v] = vt;
                        sz[v] = set.size();
                    }
                }
            }
        }
    }
    // for(int i = 1; i <= G.n; i ++){
    //     std::cout << sz[i] << " ";
    // }
    for(int i = 1; i <= G.n; i ++){
        if(F * G.A > val[i]){
            fout << i << " " << F << " " << b << "\n";
        }
    }
}