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

void UpdateLayer(const Graph &G, Layer &L, const VI &updedg){
    auto cmp = [&](const int &x, const int &y){
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
        return false;
    };
    auto Gt_new = [&](int x, int y){
        return false;
    };

    for(auto eid : updedg){
        Qseed.insert(eid);
    }
    // from line 7
    while(!Qseed.empty()){
        auto eid = *Qseed.begin();
        int l = L.layernum[eid];
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
                    }
                }
            } 
        }
        Qkp.insert(eid);
    }
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
            if(!Qseed.count(ee) && !Qkp.count(ee)){
                continue;
            }
            int w = G.edg[ee].first ^ G.edg[ee].second ^ u;
            if(!hash_table.count(1LL * v * G.n + w)){
                continue;
            }
            L.sesup[ee] --; // update ?
            // if(L.sesup[ee] <= G.prop.k - 2){
            //     L.layernum[ee] = l;
            // }
        }
    }
    for(auto ee : Qkp){
        lnum_new[ee] = 1;
    }
    // copy L' and sesup' to L and sesup
}

void UpdateUB(){}