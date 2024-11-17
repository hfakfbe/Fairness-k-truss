#include "Truss.h"

Layer compute_layer(const Graph& G, int q, int F){
    int n = G.n, m = G.m, k = G.prop.k;
    Layer L;
    L.layernum.resize(m);
    std::fill(L.layernum.begin(), L.layernum.end(), -1);

    VI sup(m);
    VI viddo(n + 1); // vertices_in_degree_decending_order
    std::vector<std::unordered_set<int>> A(n + 1);
    hash_map hash_table(n, m);
    edge_link e_link(n, m);
    for(int i = 0; i < m; i ++){
        auto [u, v] = G.edg[i];
        hash_table[1LL * u * n + v] = i;
        hash_table[1LL * v * n + u] = i;
        e_link.insert(u, v, i);
        e_link.insert(v, u, i);
    }

    for(int i = 1; i <= n; i ++) viddo[i] = i;
    auto cmp = [&] (const int & v1, const int & v2) -> bool {
        return G.prop.deg[v1] != G.prop.deg[v2] ? G.prop.deg[v1] > G.prop.deg[v2] : v1 < v2;
    };
    sort(viddo.begin() + 1, viddo.begin() + n + 1, cmp);
    for(int i = 1; i <= n; i ++){
        int u = viddo[i];
        for(auto eid : G.adj[u]){ 
            if(!G.prop.ksubtruss[eid]){
                continue;
            }
            int v = G.edg[eid].first ^ G.edg[eid].second ^ u;
            if(cmp(v, u)){
                continue;
            } //如果v已经被访问过，continue
            for(auto w : A[u]){
                if(A[v].count(w)){
                    int evw = hash_table[1ll * v * n + w];
                    if(!G.prop.ksubtruss[evw]){
                        continue;
                    }
                    int euw = hash_table[1ll * u * n + w];
                    if(!G.prop.ksubtruss[euw]){
                        continue;
                    }
                    sup[eid] ++;
                    sup[euw] ++;
                    sup[evw] ++;
                }
            }
            A[v].insert(u);
        }
    }
    //以上逻辑：不重复地枚举每个(w, u, v)三角形，作为三条边的support数量

    VI P, removed_edges(m);
    for(int i = 0; i < m; i ++){
        if(G.prop.ksubtruss[i] && !G.prop.ktruss[i] && sup[i] < k - 2){ //anchored的边不能进队
            // removed_edges[i] = true;
            P.push_back(i);
        }
    }
    if(P.empty()){ //(k-1)-truss和k-truss重合
        return L;
    }
    //解决两条边都在layer1导致不降sup的bug，所以用邻接链表删边
    L.layer.resize(1);
    for(int i = 1; P.size(); i ++){
        L.layer.push_back(VI());

        VI newP;
        for(auto eid : P){
            if(removed_edges[eid]){
                continue;
            }
            removed_edges[eid] = true;
            L.layernum[eid] = L.layer.size() - 1;
            L.layer.back().push_back(eid);

            auto [u, v] = G.edg[eid];
            hash_table.erase(1ll * v * n + u);
            hash_table.erase(1ll * u * n + v);
            e_link.erase(eid << 1);
            e_link.erase(eid << 1 | 1);

            if(G.adj[u].size() > G.adj[v].size()){
                std::swap(u, v);
            }
            for(int i = e_link.p[u]; ~i; i = e_link.e[i].next) {
                // 我们要保证三角形是存在的
                int euw = e_link.e[i].index_in_e;
                if(!G.prop.ksubtruss[euw] || removed_edges[euw]){ 
                    continue;
                } 
                int w = e_link.e[i].v;
                if(w == v || !hash_table.count(1ll * v * n + w)){ 
                    continue;
                }
                int evw = hash_table[1ll * v * n + w];
                if(!G.prop.ksubtruss[evw] || removed_edges[evw]){
                    continue;
                }
                // 去除这条边影响
                if(-- sup[euw] < k - 2 && !G.prop.ktruss[euw]){
                    newP.push_back(euw);
                }
                if(-- sup[evw] < k - 2 && !G.prop.ktruss[evw]){
                    newP.push_back(evw);
                }
            }
        }
        P.clear();
        for(auto eid : newP){
            if(!removed_edges[eid]){
                P.push_back(eid);
            }
        }
    }

    for(int i = 0; i < G.m; i ++){
        if(!G.prop.ktruss[i] && !G.prop.ktrussnoq[i] && L.layernum[i] == -1){
            continue;
        }
        auto [u, v] = G.edg[i];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        for(auto euw : G.adj[u]){
            if(G.prop.ktruss[euw] || G.prop.ktrussnoq[euw] || L.layernum[euw] != -1){
                continue;
            }
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(w == v || !hash_table.count(1LL * v * n + w)){
                continue;
            }
            int evw = hash_table[1LL * v * n + w];
            if(G.prop.ktruss[evw] || G.prop.ktrussnoq[evw] || L.layernum[evw] != -1){
                continue;
            }
            L.layer[0].push_back(euw);
            L.layer[0].push_back(evw);
            L.layernum[euw] = L.layernum[evw] = 0;
        }
    }

    L.UB = compute_UB(G, L, q, F);
    return L;
}

VI compute_UB(const Graph& G, const Layer& L, int q, int F){
    int n = G.n, m = G.m, k = G.prop.k;
    int attrcnt = G.A;
    VI UB(n + 1);
    VI origin_attribute(attrcnt);

    struct GetEdgeId{
        std::unordered_map<i64, int> ehash;
        void insert(int u, int v, int id){
            if(u > v){
                std::swap(u, v);
            }
            ehash[1LL * u * n + v] = id;
        }
        int get(int u, int v){
            if(u > v){
                std::swap(u, v);
            }
            return ehash[1LL * u * n + v];
        }
        bool count(int u, int v){
            if(u > v){
                std::swap(u, v);
            }
            return ehash.count(1LL * u * n + v);
        }
        int n;
    } geteid;
    geteid.n = n;

    for(int i = 0; i < m; i ++){
        geteid.insert(G.edg[i].first, G.edg[i].second, i);
    }

    std::vector<std::unordered_set<int>> pointset(m), extcompointset(m),
        pointsetext(m);
    VI extcom(n + 1), vis(m), access(m);
    std::fill(origin_attribute.begin(), origin_attribute.end(), 0);

    //先计算所有外部社区的点
    for(int i = 0; i < m; i ++){
        if(G.prop.ktrussnoq[i]){
            extcom[G.edg[i].first] = extcom[G.edg[i].second] = G.prop.ktrussnoq[i];
            extcompointset[G.prop.ktrussnoq[i]].insert(G.edg[i].first);
            extcompointset[G.prop.ktrussnoq[i]].insert(G.edg[i].second);
        }else if(G.prop.ktruss[i]){
            extcom[G.edg[i].first] = extcom[G.edg[i].second] = -1;
        }
    }

    int ori_val = compute_value(G, G.prop.ktruss, F);

    // 计算边，使用类似拓扑排序的方法，先计算下层点的度数，从高层往低层推，当所有高层点都进行完毕，再进行低层
    VI layer_deg(m + 1);
    for(int i = 0; i < G.m; i ++){
        if(L.layernum[i] == -1){
            continue;
        }
        auto [u, v] = G.edg[i];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        for(auto euw : G.adj[u]){
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(w != v && geteid.count(v, w)){
                int evw = geteid.get(v, w);
                if(G.prop.ktruss[evw] || L.layernum[evw] > L.layernum[i]){
                    layer_deg[i] ++;
                }
                if(G.prop.ktruss[euw] || L.layernum[euw] > L.layernum[i]){
                    layer_deg[i] ++;
                }
            }
        }
    }
    // for(int i = 0; i < G.m; i ++){
    //     std::cerr << layer_deg[i] << " \n"[i == G.m - 1];
    // }
    std::queue<int> Q;
    for(int i = 0; i < m; i ++){
        if(G.prop.ktruss[i]){
            Q.push(i);
            access[i] = 1;
        }else if(G.prop.ksubtruss[i] && !G.prop.ktrussnoq[i] && layer_deg[i] == 0){
            Q.push(i);
            access[i] = 0;
        }
    }
    while(!Q.empty()){
        auto eid = Q.front();
        Q.pop();
        auto [u, v] = G.edg[eid];
        // 插入当前点
        if(!G.prop.ktruss[eid]){
            for(auto uv : {u, v}){
                if(extcom[uv] >= 0){
                    if(extcom[uv]){
                        pointsetext[eid].insert(extcom[uv]);
                    }else{
                        pointset[eid].insert(uv);
                    }
                }
            }
        }
        // 遍历三角形
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        int lnum = G.prop.ktruss[eid] ? INT_MAX : L.layernum[eid];
        for(auto euw : G.adj[u]){
            // 上层
            int w = G.edg[euw].first ^ G.edg[euw].second ^ u;
            if(w != v && geteid.count(v, w)){
                int evw = geteid.get(v, w);
                if(L.layernum[evw] != -1 && L.layernum[evw] < lnum){
                    pointset[evw].insert(pointset[eid].begin(), pointset[eid].end());
                    pointsetext[evw].insert(pointsetext[eid].begin(), pointsetext[eid].end());
                    if(-- layer_deg[evw] == 0){
                        Q.push(evw);
                    }
                    access[evw] |= access[eid];
                }
                if(L.layernum[euw] != -1 && L.layernum[euw] < lnum){
                    pointset[euw].insert(pointset[eid].begin(), pointset[eid].end());
                    pointsetext[euw].insert(pointsetext[eid].begin(), pointsetext[eid].end());
                    if(-- layer_deg[euw] == 0){
                        Q.push(euw);
                    }
                    access[euw] |= access[eid];
                }
            }
        }
    }
    
#ifdef debug
    for(int i = 0; i < G.m; i ++){
        if(layer_deg[i] != 0){
            std::cerr << "Graph:\n";
            for(auto [u, v] : G.edg){
                std::cerr << u << " " << v << "\n";
            }
            std::cerr << "q and F:\n";
            std::cerr << q << " " << F << "\n";
            std::cerr << "layer_deg:\n";
            for(int i = 0; i < G.m; i ++){
                std::cerr << layer_deg[i] << " \n"[i == G.m - 1];
            }
            assert(0);
        }
    }
#endif

    // 计算点
    for(int i = 1; i <= n; i ++){
        if(extcom[i] < 0)
            continue;
        std::unordered_set<int> pts, ptsext;
        int isaccess = 0;
        for(auto eid : G.adj[i]){
            //合并
            isaccess |= access[eid];
            pts.insert(pointset[eid].begin(), pointset[eid].end());
            ptsext.insert(pointsetext[eid].begin(), pointsetext[eid].end());
        }

        //计算 val
        if(isaccess){
            VI attrval(attrcnt);
            for(auto idc : ptsext){
                pts.insert(extcompointset[idc].begin(), extcompointset[idc].end());
            }
            for(const auto& v : pts){
                attrval[G.attr[v]] ++;
            }
            UB[i] = - ori_val;
            for(int j = 0; j < attrcnt; j ++){
                UB[i] += std::max(attrval[j], F);
            }
        }
    }
        // for(int i = 1; i <= n; i ++){
        //     std::cerr << UB[i] << " \n"[i == n];
        // }
    return UB;
}

