#include "Truss.h"

VI compute_triangle_three_truss(const Graph& G){
    VI dsu(G.m), rank(G.m);
    hash_map hash_table(G.n, G.m);
    for(int i = 0; i < G.m; i ++){
        dsu[i] = i;
        auto [u, v] = G.edg[i];
        hash_table[1LL * u * G.n + v] = i;
        hash_table[1LL * v * G.n + u] = i;
    }
    std::function<int(int)> find = [&](int x){
        return dsu[x] == x ? x : dsu[x] = find(dsu[x]);
    };
    auto merge = [&](int x, int y){
        x = find(x), y = find(y);
        if(x != y){
            if(rank[y] > rank[x]){
                std::swap(x, y);
            }
            dsu[x] = y;
            if(rank[y] == rank[x]){
                rank[y] ++;
            }
        }
    };
    
    for(int i = 0; i < G.m; i ++){
        auto [u, v] = G.edg[i];
        for(auto euw : G.adj[u]){
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(w == v || !hash_table.count(1ll * v * G.n + w)){
                continue;
            }
            int evw = hash_table[1ll * v * G.n + w];
            merge(i, euw);
            merge(euw, evw);
        }
    }
    for(int i = 0; i < G.m; i ++){
        dsu[i] = find(i);
    }
    return dsu;
}

VI compute_maximal_ktruss(const Graph& G, const VI& tritruss, int k, int q){
    VI vise(G.m), visv(G.n + 1), vist(G.m);
    VVI tritruss_group(G.m);
    std::queue<int> Q;
    for(int i = 0; i < G.m; i ++){
        tritruss_group[tritruss[i]].push_back(i);
    }
    Q.push(q);
    visv[q] = 1;
    while(true){
        VI Q_delta;
        while(!Q.empty()){
            auto u = Q.front();
            Q.pop();
            for(auto eid : G.adj[u]){
                if(!vise[eid] && G.prop.trussness[eid] >= k){
                    vise[eid] = 1;
                    if(!vist[tritruss[eid]]){
                        vist[tritruss[eid]] = 1;
                        Q_delta.push_back(tritruss[eid]);
                    }
                    int v = G.edg[eid].first ^ G.edg[eid].second ^ u;
                    if(!visv[v]){
                        visv[v] = 1;
                        Q.push(v);
                    }
                }
            }
        }
        if(Q_delta.empty()){
            break;
        }else{
            for(auto tid : Q_delta){
                for(auto eid : tritruss_group[tid]){
                    if(!vise[eid]){
                        vise[eid] = 1;
                        auto [u, v] = G.edg[eid];
                        for(auto uv : {u, v}){
                            if(!visv[uv]){
                                visv[uv] = 1;
                                Q.push(uv);
                            }
                        }
                    }
                }
            }
        }
    }
    return vise;
}

std::pair<Graph, int> compute_maxk(const Graph& G, int q, int F){
    auto tritruss = compute_triangle_three_truss(G);

    int l = 2, r = 2;
    for(auto eid : G.adj[q]){
        r = std::max(r, G.prop.trussness[eid]);
    }

    auto check = [&](int x){
        VI res = compute_maximal_ktruss(G, tritruss, x, q);
        int cnt = 0;
        for(int i = 0; i < G.m; i ++){
            cnt += res[i];
        }
        std::cerr << cnt << "\n";
        return compute_value(G, res, F) == F * G.A;
    };

    while(l < r){
        int mid = l + r + 1 >> 1;
        std::cerr << "check: " << mid << "\n";
        if(check(mid)){
            l = mid;
        }else{
            r = mid - 1;
        }
    }

    VI res = compute_maximal_ktruss(G, tritruss, l, q);
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
    G2.prop.k = l;
    G2.prop = update_property(G2, res, q, F);

    return {G2, l};
}