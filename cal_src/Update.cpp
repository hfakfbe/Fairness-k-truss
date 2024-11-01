#include "Truss.h"

hash_map hash_table;

void UpdataInit(const Graph &G){
    hash_table = hash_map(G.n, G.m);
    for(int i = 0; i < G.m; i ++){
        auto [u, v] = G.edg[i];
        hash_table[1LL * u * G.n + v] = i;
        hash_table[1LL * v * G.n + u] = i;
    }
}

/**
 * @brief Updates the layer information of a graph based on updated edges.
 * 
 * This function updates the layer numbers and support numbers of edges in a graph,
 * considering a specific set of updated edges. It uses several data structures to
 * efficiently process and update the graph's layering and support information.
 *
 * @param G         The input graph, represented by a custom Graph type.
 * @param L         The layer information to be updated, represented by a Layer type.
 * @param updedg    A vector of edge IDs that have been updated and need re-evaluation.
 */
void update_layer(Graph &G, const std::set<int> &updedg){
    auto &L = G.prop.layer;
    auto cmp = [&](const int &x, const int &y){
        // std::cout << "cmp: " << x << " " << y << "\n";
        if(L.layernum[x] == L.layernum[y]){
            return x < y;
        }
        return L.layernum[x] < L.layernum[y];
    };
    std::map<int, int> sesup_new, lnum_new;
    std::set<int, decltype(cmp)> Qseed(cmp), Qk(cmp), Qkp(cmp);
    auto Geq = [&](int x, int y){
        if(G.prop.trussness[x] > G.prop.trussness[y]){
            return true;
        }else if(G.prop.trussness[x] == G.prop.trussness[y]){
            return L.layernum[x] >= L.layernum[y];
        }
        return false;
    };
    auto Gt = [&](int x, int y){
        if(G.prop.trussness[x] > G.prop.trussness[y]){
            return true;
        }else if(G.prop.trussness[x] == G.prop.trussness[y]){
            return L.layernum[x] > L.layernum[y];
        }
        return false;
    };
    auto Gt_new = [&](int x, int y){
        int taux = updedg.count(x) ? G.prop.k : G.prop.trussness[x];
        int tauy = updedg.count(y) ? G.prop.k : G.prop.trussness[y];
        assert(std::min(taux, tauy) < G.prop.k); // check if resonable
        if(taux > tauy){
            return true;
        }else if(taux == tauy){
            int lx = lnum_new.count(x) ? lnum_new[x] : L.layernum[x];
            int ly = lnum_new.count(y) ? lnum_new[y] : L.layernum[y];
            return lx > ly;
        }
        return false;
    };

    for(auto eid : updedg){
        // std::cout << eid << " eid\n";
        Qseed.insert(eid);
    }
    // from line 7
    int l = 0;
    while(!Qseed.empty()){
        auto eid = *Qseed.begin();
        l = L.layernum[eid];
        if(!Qk.empty() && L.layernum[*Qk.begin()] < l){
            auto ek = *Qk.begin();
            Qk.erase(Qk.begin());
            // for all e' \in Qseed or Qkp forming triangle
            auto [u, v] = G.edg[ek];
            if(G.adj[u].size() > G.adj[v].size()){
                std::swap(u, v);
            }
            for(auto ee : G.adj[u]){
                if(!Qseed.count(ee) && !Qkp.count(ee)){
                    continue;
                }
                int w = G.edg[ee].first ^ G.edg[ee].second ^ u;
                if(!hash_table.count(1LL * v * G.n + w)){
                    continue;
                }
                L.sesup[ee] --; // update ?
                if(L.sesup[ee] <= G.prop.k - 2){
                    L.layernum[ee] = l;
                    Qseed.insert(ee);
                    Qkp.insert(ee);
                }
            }
            continue;   
        }
        lnum_new[eid] = INT_MAX;
        Qseed.erase(Qseed.begin());
        // from line 18
        auto [u, v] = G.edg[eid];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        for(auto euw : G.adj[u]){
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(!hash_table.count(1LL * v * G.n + w)){
                continue;
            }
            int evw = hash_table[1LL * v * G.n + w];
            if(std::min(G.prop.trussness[euw], G.prop.trussness[evw]) != G.prop.k || !Geq(euw, eid) || !Geq(evw, eid)){
                continue;
            }
            if(Gt(euw, eid) && Gt(evw, eid)){
                if(Gt_new(eid, euw) && Gt_new(eid, evw)){
                    for(auto ee : {euw, evw}){
                        sesup_new[ee] ++;
                        if(sesup_new[ee] > G.prop.k - 2){
                            Qseed.insert(ee);
                        }
                    }
                }
            }else{
                for(auto ee : {euw, evw}){
                    if(G.prop.trussness[ee] == G.prop.k && L.layernum[ee] == l){
                        Qk.insert(ee);
                        // 为什么这里的边可以放到Qk里，但是前面由于Qk而sesup改变到<k-2的Qseed、Qkp的边直接删除，而不是进入Qk里？
                    }
                }
            } 
        }
        Qkp.insert(eid);
    }
    // 原文这里是if，是应该是while吧？
    while(!Qk.empty()){
        // line 29
        auto ek = *Qk.begin();
        Qk.erase(Qk.begin());
        auto [u, v] = G.edg[ek];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        // for all e' \in Qseed or Qkp forming triangle
        for(auto ee : G.adj[u]){
            // Qseed is empty ?
            if(!Qseed.count(ee) && !Qkp.count(ee)){ // 实际上不需要判断Qseed
                continue;
            }
            int w = G.edg[ee].first ^ G.edg[ee].second ^ u;
            if(!hash_table.count(1LL * v * G.n + w)){
                continue;
            }
            L.sesup[ee] --; // update ?
            if(L.sesup[ee] <= G.prop.k - 2){
                L.layernum[ee] = l + 1; // 是正确的吗?
                Qkp.erase(ee);
            }
        }
    }
    // check the correctness
    assert(Qkp.size() == updedg.size());
    for(auto eid : updedg){
        assert(Qkp.count(eid));
    }

    // compare the diff
    // std::cout << "lnum_old: ";
    // for(int i = 0; i < G.m; i ++){
    //     std::cout << L.layernum[i] << " \n"[i == G.m - 1];
    // }

    // update the layer number
    for(auto [eid, ln] : lnum_new){
        L.layernum[eid] = ln;
    }
    for(auto eid : updedg){
        L.layernum[eid] = -1;
    }
    for(auto [eid, ss] : sesup_new){
        L.sesup[eid] = ss;
    }

    // std::cout << "lnum_new: ";
    // for(int i = 0; i < G.m; i ++){
    //     std::cout << L.layernum[i] << " \n"[i == G.m - 1];
    // }
}

void UpdateUB(){}