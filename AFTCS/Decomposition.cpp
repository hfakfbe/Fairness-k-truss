#include "Truss.h"

VI decomposition(const Graph& G){
	VI sup(G.m), trussness(G.m);
    hash_map hash_table(G.n, G.m);
    edge_link e_link(G.n, G.m);
    for(int i = 0; i < G.m; i ++){
        auto [u, v] = G.edg[i];
        hash_table[1LL * u * G.n + v] = i;
        hash_table[1LL * v * G.n + u] = i;
        e_link.insert(u, v, i);
        e_link.insert(v, u, i);
    }

    int max_trussness = *std::max_element(G.prop.deg.begin(), G.prop.deg.end()) + 2;
    std::vector<std::unordered_set<int>> trussness_class(max_trussness);
    VI viddo(G.n + 1);
    for(int i = 1; i <= G.n; i++){
		viddo[i] = i;
	}
    auto cmp = [&] (const int & v1, const int & v2) -> bool {
        return G.prop.deg[v1] != G.prop.deg[v2] ? G.prop.deg[v1] > G.prop.deg[v2] : v1 < v2;
    };
    std::sort(viddo.begin() + 1, viddo.begin() + G.n + 1, cmp);
    std::vector<std::unordered_set<int>> A(G.n + 1);

    for(int i = 1; i <= G.n; i++){
        int u = viddo[i];
        for(auto eid : G.adj[u]){
            int v = G.edg[eid].first ^ G.edg[eid].second ^ u;
            if(cmp(v, u)){
                continue;
            }
            for(const int & w : A[u]){
                if(A[v].count(w)){ 
                    sup[eid] ++;
                    sup[hash_table[1ll * v * G.n + w]] ++;
                    sup[hash_table[1ll * u * G.n + w]] ++;
                }
            }
            A[v].insert(u);
        }
    }

    VI removed_edges(G.m);
    
    int cur_minimum_trussness = max_trussness, sub_minimum_trussness = max_trussness;
    
    for(int i = 0; i < G.m; i++){
        trussness_class[sup[i] + 2].insert(i);
        if(sup[i] < cur_minimum_trussness - 2){
            sub_minimum_trussness = cur_minimum_trussness; 
            cur_minimum_trussness = sup[i] + 2; 
        }
    }
    std::queue<int> Q;
    
    for(int i = 0; i < G.m; i++){
        if(sup[i] == cur_minimum_trussness - 2){
            Q.push(i);
            removed_edges[i] = true;
            trussness[i] = cur_minimum_trussness;
        }
    }
    cur_minimum_trussness = sub_minimum_trussness;
	
    while(!Q.empty()){
		auto [u, v] = G.edg[Q.front()];
        Q.pop();
        int eid_uv = hash_table[1ll * u * G.n + v];
        hash_table.erase(1ll * v * G.n + u);
        hash_table.erase(1ll * u * G.n + v);
        e_link.erase(eid_uv << 1);
		e_link.erase(eid_uv << 1 | 1);

		for(auto uv : {u, v}){
			for(int i = e_link.p[u ^ v ^ uv]; ~i; i = e_link.e[i].next) {
				int eid = e_link.e[i].index_in_e;
				if(removed_edges[eid]){ 
                    continue; 
                }
				int w = e_link.e[i].v;
				if(w == uv){
                    continue;
                }
				if(hash_table.count(1ll * uv * G.n + w)){
					trussness_class[sup[eid] + 2].erase(eid); 
					sup[eid] --;
					trussness_class[sup[eid] + 2].insert(eid);
					if(sup[eid] <= trussness[eid_uv] - 2 && !removed_edges[eid]){
						removed_edges[eid] = true;
						trussness[eid] = trussness[eid_uv];
						Q.push(eid);
					}
				}
			}
		}

        if(Q.empty()){
            for(int i = trussness[eid_uv] + 1; i < max_trussness; i++){
                if((int) trussness_class[i].size() != 0){
                    for(const int & eid : trussness_class[i]){
                        Q.push(eid);
                        removed_edges[eid] = true;
                        trussness[eid] = i;
                    }
                    break;
                }
            }
        }
    }
    return trussness;
}