#include "Truss.h"

hash_map hash_table;

void update_init(const Graph &G){
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
    std::map<int, int> sesup_new, lnum_new;

    auto get_lnum_new = [&](int eid){
        if(G.prop.trussness[eid] >= G.prop.k){
            return INT_MAX;
        }
        if(lnum_new.count(eid) && lnum_new[eid] > 0){
            return lnum_new[eid];
        }
        return L.layernum[eid];
    };
    auto cmp = [&](const int &x, const int &y){
        // std::cout << "cmp: " << x << " " << y << "\n";
        if(L.layernum[x] == L.layernum[y]){
            return x < y;
        }
        return L.layernum[x] < L.layernum[y];
    };
    std::set<int, decltype(cmp)> Qseed(cmp), Qk(cmp), Qkp(cmp);

    for(auto eid : updedg){
        // std::cout << eid << " eid\n";
        if(G.prop.trussness[eid] < G.prop.k){
            Qseed.insert(eid);
        }
    }
    // assert(!lnum_new.count(1) || lnum_new[1] != 0);
    // from line 7
    int l = 0;
    while(!Qseed.empty()){
        auto eid = *Qseed.begin();
        l = L.layernum[eid];
        if(!Qk.empty() && lnum_new[*Qk.begin()] < l){
            auto ek = *Qk.begin();
            Qk.erase(Qk.begin());
            // for all e' \in Qseed or Qkp forming triangle
            auto [u, v] = G.edg[ek];
            if(G.adj[u].size() > G.adj[v].size()){
                std::swap(u, v);
            }
            for(auto euw : G.adj[u]){
                int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
                if(!hash_table.count(1LL * v * G.n + w)){
                    continue;
                }
                int evw = hash_table[1LL * v * G.n + w];
                for(auto ee : {euw, evw}){
                    if(!Qseed.count(ee) && !Qkp.count(ee)){
                        continue;
                    }
                    // std::cout << ek << " " << ee << " here\n";
                    // std::cout << L.layernum[ek] << " " << L.layernum[ee] << " " << get_lnum_new(ek) << " " << get_lnum_new(ee) << "\n";
                    // if(L.layernum[ek] >= L.layernum[ee]){
                    //     L.sesup[ee] --; // update ?
                    // }else if(get_lnum_new(ek) >= get_lnum_new(ee)){
                    //     L.sesup[ee] --;
                    // }
                    if(get_lnum_new(ek) >= L.layernum[ee]){
                        L.sesup[ee] --;
                    }
                    if(L.sesup[ee] < G.prop.k - 2 && !updedg.count(ee)){
                        lnum_new[ee] = l;
                        Qseed.erase(ee);
                        Qkp.erase(ee);
                        Qk.insert(ee);
                    }
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
            int lnum_uv = G.prop.trussness[eid] >= G.prop.k ? INT_MAX : L.layernum[eid];
            int lnum_vw = G.prop.trussness[evw] >= G.prop.k ? INT_MAX : L.layernum[evw];
            int lnum_uw = G.prop.trussness[euw] >= G.prop.k ? INT_MAX : L.layernum[euw];
            if(std::min(G.prop.trussness[euw], G.prop.trussness[evw]) != G.prop.k - 1 || lnum_uv > lnum_uw || lnum_uv > lnum_vw){
                continue;
            }
            if(lnum_uv < lnum_uw && lnum_uv < lnum_vw){
                int lnum_new_uv = get_lnum_new(eid);
                int lnum_new_uw = get_lnum_new(euw);
                int lnum_new_vw = get_lnum_new(evw);
                if(lnum_new_uw <= lnum_new_uv && lnum_new_uw <= lnum_new_vw && G.prop.trussness[euw] < G.prop.k){
                    L.sesup[euw] ++;
                    if(L.sesup[euw] >= G.prop.k - 2){
                        Qseed.insert(euw);
                    }
                }
                if(lnum_new_vw <= lnum_new_uv && lnum_new_vw <= lnum_new_uw && G.prop.trussness[evw] < G.prop.k){
                    L.sesup[evw] ++;
                    if(L.sesup[evw] >= G.prop.k - 2){
                        Qseed.insert(evw);
                    }
                }
            }else{
                for(auto ee : {euw, evw}){
                    if(G.prop.trussness[ee] == G.prop.k - 1 && L.layernum[ee] == l && !updedg.count(ee)){
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
        for(auto euw : G.adj[u]){
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(!hash_table.count(1LL * v * G.n + w)){
                continue;
            }
            int evw = hash_table[1LL * v * G.n + w];
            for(auto ee : {euw, evw}){
                // Qseed is empty ?
                if(!Qseed.count(ee) && !Qkp.count(ee)){
                    continue;
                }
                L.sesup[ee] --; // update ?
                if(L.sesup[ee] < G.prop.k - 2 && !updedg.count(ee)){
                    lnum_new[ee] = l;
                    Qkp.erase(ee);
                }
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
        if(ln > 0){
            L.layernum[eid] = ln;
        }
    }
    for(auto eid : updedg){
        L.layernum[eid] = -1;
        L.sesup[eid] = -1;
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