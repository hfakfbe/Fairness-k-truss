#include "Truss.h"

Property compute_property(const Graph& G, int q, int F){
    Property res = G.prop;

    // deg
    res.deg.resize(G.n + 1);
    for(int i = 1; i <= G.n; i ++){
        res.deg[i] = G.adj[i].size();
    }

    // ktruss
    res.ktruss.resize(G.m);
    std::fill(res.ktruss.begin(), res.ktruss.end(), 0);
    std::queue<int> Q;
    for(auto eid : G.adj[q]){
        if(res.trussness[eid] >= res.k){
            res.ktruss[eid] = 1;
            Q.push(eid);
        }
    }
    while(!Q.empty()){
        auto [u, v] = G.edg[Q.front()];
        Q.pop();
        for(auto uv : {u, v}){
            for(auto eid : G.adj[uv]){
                if(res.trussness[eid] >= res.k && !res.ktruss[eid]){
                    res.ktruss[eid] = 1;
                    Q.push(eid);
                }
            }
        }
    }

    // ktrussnoq
    res.ktrussnoq.resize(G.m);
    std::fill(res.ktrussnoq.begin(), res.ktrussnoq.end(), 0);
    int tot = 1;
    std::stack<int> S;
    for(int i = 0; i < G.m; i ++){
        if(res.trussness[i] >= res.k && !res.ktruss[i]){
            S.push(i);
        }
    }
    while(!S.empty()){
        auto eid = S.top();
        S.pop();
        if(!res.ktrussnoq[eid]){
            res.ktrussnoq[eid] = ++ tot;
        }
        auto [u, v] = G.edg[eid];
        for(auto uv : {u, v}){
            for(auto eid2 : G.adj[uv]){
                if(res.trussness[eid2] >= res.k && !res.ktruss[eid2] && !res.ktrussnoq[eid2]){
                    res.ktrussnoq[eid2] = res.ktrussnoq[eid];
                    S.push(eid2);
                }
            }
        }
    }

    // ksubtruss
    res.ksubtruss.resize(G.m);
    std::fill(res.ksubtruss.begin(), res.ksubtruss.end(), 0);
    // for(auto eid : G.adj[q]){
    //     if(res.trussness[eid] >= res.k - 1 ){
    //         Q.push(eid);
    //         res.ksubtruss[eid] = 1;
    //     }
    // }
    for(int i = 0; i < G.m; i ++){
        if(res.trussness[i] >= res.k - 1){
            // Q.push(i);
            res.ksubtruss[i] = 1;
        }
    }
    // while(!Q.empty()){
    //     auto [u, v] = G.edg[Q.front()];
    //     Q.pop();
    //     for(auto uv : {u, v}){
    //         for(auto eid : G.adj[uv]){
    //             if(res.trussness[eid] >= res.k - 1 && !res.ksubtruss[eid]){
    //                 res.ksubtruss[eid] = 1;
    //                 Q.push(eid);
    //             }
    //         }
    //     }
    // }
    // for(int i = 0; i < G.m; i ++){
    //     if(res.ktrussnoq[i] || res.ktruss[i]){
    //         res.ksubtruss[i] = 0;
    //     }
    // }

    return res;
}

Property update_property(const Graph& G, VI& T, int q, int F){
    Property res = G.prop;

    // ktruss
    res.ktruss = T;

    // ktrussnoq
    res.ktrussnoq.resize(G.m);
    std::fill(res.ktrussnoq.begin(), res.ktrussnoq.end(), 0);
    for(int i = 0; i < G.m; i ++){
        if(res.ktruss[i]){
            res.ktrussnoq[i] = 0;
        }
    }

    // ksubtruss
    res.ksubtruss.resize(G.m);
    std::queue<int> Q;
    std::fill(res.ksubtruss.begin(), res.ksubtruss.end(), 0);
    // for(int i = 0; i < G.m; i ++){
    //     if(res.ktruss[i]){
    //         auto [u, v] = G.edg[i];
    //         for(auto uv : {u, v}){
    //             for(auto eid : G.adj[uv]){
    //                 if(res.trussness[eid] >= res.k - 1 && !res.ksubtruss[eid]){
    //                     res.ksubtruss[eid] = 1;
    //                     Q.push(eid);
    //                 }
    //             }
    //         }
    //     }
    // }
    for(int i = 0; i < G.m; i ++){
        if(res.trussness[i] >= res.k - 1){
            // Q.push(i);
            res.ksubtruss[i] = 1;
        }
    }
    // while(!Q.empty()){
    //     auto [u, v] = G.edg[Q.front()];
    //     Q.pop();
    //     for(auto uv : {u, v}){
    //         for(auto eid : G.adj[uv]){
    //             if(res.trussness[eid] >= res.k - 1 && !res.ksubtruss[eid]){
    //                 res.ksubtruss[eid] = 1;
    //                 Q.push(eid);
    //             }
    //         }
    //     }
    // }
    // for(int i = 0; i < G.m; i ++){
    //     if(res.ktrussnoq[i] || res.ktruss[i]){
    //         res.ksubtruss[i] = 0;
    //     }
    // }

    return res;
}

int compute_value(const Graph& G, const VI& T, int F){
    std::unordered_set<int> pointset;
    for(int i = 0; i < G.m; i ++){
        if(T[i]){
            pointset.insert(G.edg[i].first);
            pointset.insert(G.edg[i].second);
        }
    }
    VI count(G.A);
    for(auto v : pointset){
        count[G.attr[v]] ++;
    }
    int res = 0;
    for(auto a : count){
        res += std::min(a, F);
    }
    return res;
}
