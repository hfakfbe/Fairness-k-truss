#include "Truss.h"

#define SURVIVED 1
#define DISCARDED 2

VI extcom, access, vis;
std::vector<std::unordered_set<int>> extcompointset;
VI status, s_plus, edge_anchored, inqueue;

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

void compute_anchor_init(const Graph& G){
    extcom.resize(G.n + 1), access.resize(G.m), vis.resize(G.m);
    extcompointset.resize(G.m);
    status.resize(G.m), s_plus.resize(G.m), edge_anchored.resize(G.m), inqueue.resize(G.m);

    for(int i = 0; i < G.m; i ++){
        if(G.prop.ktrussnoq[i]){
            extcom[G.edg[i].first] = extcom[G.edg[i].second] = G.prop.ktrussnoq[i];
            extcompointset[G.prop.ktrussnoq[i]].insert(G.edg[i].first);
            extcompointset[G.prop.ktrussnoq[i]].insert(G.edg[i].second);
        }else if(G.prop.ktruss[i]){
            extcom[G.edg[i].first] = extcom[G.edg[i].second] = -1;
        }
    }
    geteid.n = G.n;
    for(int i = 0; i < G.m; i ++){
        geteid.insert(G.edg[i].first, G.edg[i].second, i);
    }
}

VI compute_follower(const Graph& G, const Layer& L, int q, int F, int anchor){
    auto& D = G.prop.deg;
    auto& ktruss = G.prop.ktruss;
    auto& ktrussnoq = G.prop.ktrussnoq;
    auto& e = G.edg;
    auto& attr = G.attr;
    auto& layernum_of_edge = L.layernum;
    int n = G.n, m = G.m, k = G.prop.k;
    VI followers;
    
    std::unordered_set<int> result, followset;

    if(extcom[anchor] < 0){
        return VI();
    }

    auto cmp = [&](const int x, const int y){
        return layernum_of_edge[x] > layernum_of_edge[y];
    };
    std::priority_queue<int, VI, std::function<bool(const int&, const int&)>> H(cmp);

    for(auto eid : G.adj[anchor]){
        if(layernum_of_edge[eid] == -1){
            //说明不在layer中，一定没有follower
            //如果在layer中，一定可以连到q吗？
            continue;
        }
        edge_anchored[eid] = 1;
        inqueue[eid] = 1;
        H.push(eid);
    }


    int tot2 = 0;

    std::function<void(int)> shrink = [&](int eid){
        tot2 ++;
        VI T;
        auto [u, v] = e[eid];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        for(auto euw : G.adj[u]){
            if(L.layernum[euw] <= L.layernum[eid] && !ktruss[euw] && !edge_anchored[euw]){
                continue;
            }
            int w = e[euw].first ^ e[euw].second ^ u;
            if(geteid.count(v, w)){
                int evw = geteid.get(v, w);
                if(L.layernum[evw] <= L.layernum[eid] && !ktruss[evw] && !edge_anchored[evw]){
                    continue;
                }

                if(status[euw] == SURVIVED && status[evw] != DISCARDED){
                    s_plus[euw] --;
                    if(s_plus[euw] < 1 && edge_anchored[euw]){
                        T.push_back(euw);
                    }
                    if(s_plus[euw] < k - 2 && !edge_anchored[euw]){
                        T.push_back(euw);
                    }
                }
                if(status[evw] == SURVIVED && status[euw] != DISCARDED){
                    s_plus[evw] --;
                    if(s_plus[evw] < 1 && edge_anchored[evw]){
                        T.push_back(evw);
                    }
                    if(s_plus[evw] < k - 2 && !edge_anchored[evw]){
                        T.push_back(evw);
                    }
                }
            }
        }
        for(auto eid2 : T){
            status[eid2] = DISCARDED;
            shrink(eid2);
        }
    };

    int tot = 0;

    VI opera;
    while(!H.empty()){
        int eid = H.top();
        H.pop();
        tot ++;
        opera.push_back(eid);
        //算 s+
        inqueue[eid] = 0;
        int cnt = 0;

        auto [u, v] = e[eid];
        if(G.adj[u].size() > G.adj[v].size()){
            std::swap(u, v);
        }
        for(auto euw : G.adj[u]){
            int illegal = L.layernum[euw] < L.layernum[eid] && status[euw] != SURVIVED;
            illegal |= L.layernum[euw] == L.layernum[eid] && status[euw] != SURVIVED && !inqueue[euw];
            illegal &= !ktruss[euw] && !edge_anchored[euw];
            if(illegal){
                continue;
            }
            int w = e[euw].first ^ e[euw].second ^ u;
            if(geteid.count(v, w)){
                int evw = geteid.get(v, w);
                illegal = L.layernum[evw] < L.layernum[eid] && status[evw] != SURVIVED;
                illegal |= L.layernum[evw] == L.layernum[eid] && status[evw] != SURVIVED && !inqueue[evw];
                illegal &= !ktruss[evw] && !edge_anchored[evw];
                if(illegal){
                    continue;
                }
                if(status[euw] == DISCARDED || status[evw] == DISCARDED){
                    continue;
                }
                cnt ++;
            }
        }
        
        s_plus[eid] = cnt;
        
        if(s_plus[eid] >= k - 2 || (s_plus[eid] >= 1 && edge_anchored[eid])){
            status[eid] = SURVIVED;

            for(auto euw : G.adj[u]){
                if(L.layernum[euw] <= L.layernum[eid] && !ktruss[euw] && !edge_anchored[euw]){
                    continue;
                }
                int w = e[euw].first ^ e[euw].second ^ u;
                if(geteid.count(v, w)){
                    int evw = geteid.get(v, w);
                    if(L.layernum[evw] <= L.layernum[eid] && !ktruss[evw] && !edge_anchored[evw]){
                        continue;
                    }
                    //这里考虑加in_queue
                    if(!status[euw] && !ktruss[euw] && !ktrussnoq[euw] && !inqueue[euw]){
                        inqueue[euw] = 1;
                        H.push(euw);
                    }
                    if(!status[evw] && !ktruss[evw] && !ktrussnoq[evw] && !inqueue[evw]){
                        inqueue[evw] = 1;   
                        H.push(evw);
                    }
                }
            }
        }else{
            status[eid] = DISCARDED;
            shrink(eid);
        }
    }
#ifdef debug
    std::cerr << " s+ " << tot << " shrink " << tot2;
#endif
    tot = 0;

    std::function<void(int)> dfs = [&](int eid){
        tot ++;
        opera.push_back(eid);
        vis[eid] = 1;
        if(extcom[G.edg[eid].first] == -1){
            access[eid] = 1;
        }else{
            result.insert(extcompointset[extcom[G.edg[eid].first]].begin(), extcompointset[extcom[G.edg[eid].first]].end());
        }
        if(extcom[G.edg[eid].second] == -1){
            access[eid] = 1;
        }else{
            result.insert(extcompointset[extcom[G.edg[eid].second]].begin(), extcompointset[extcom[G.edg[eid].second]].end());
        }
        if(extcom[e[eid].first] >= 0)
            result.insert(e[eid].first);
        if(extcom[e[eid].second] >= 0)
            result.insert(e[eid].second);

        auto [u, v] = e[eid];
        for(auto euw : G.adj[u]){
            if(L.layernum[euw] < L.layernum[eid] && !ktruss[euw]){
                continue;
            }
            int w = e[euw].first ^ e[euw].second ^ u;
            if(geteid.count(v, w)){
                int evw = geteid.get(v, w);
                if(L.layernum[evw] < L.layernum[eid] && !ktruss[euw]){
                    continue;
                }
                
                if(!vis[euw] && status[euw] == SURVIVED){
                    dfs(euw);
                }
                if(!vis[evw] && status[evw] == SURVIVED){
                    dfs(evw);
                }
                access[eid] |= access[euw] | access[evw];
            }
        }
    };

    int isaccess = 0;
    for(auto eid : G.adj[anchor]){
        result.clear();
        if(layernum_of_edge[eid] != -1 && !vis[eid] && status[eid] == SURVIVED){
            dfs(eid);
            isaccess |= access[eid];
            followset.insert(result.begin(), result.end());
        }
    }

#ifdef debug
    std::cerr << " dfs " << tot;
#endif

    if(!isaccess){
        followset.clear();
    }

    for(auto vertex : followset){
        followers.push_back(vertex); //包含anchor
    }

    for(auto eid : opera){
        vis[eid] = access[eid] = s_plus[eid] = status[eid] = 0;
    }
    for(auto eid : G.adj[anchor]){
        edge_anchored[eid] = 0;
    }

    return followers;
}

std::pair<VI, int> compute_anchor(const Graph& G, int q, int F){
    auto& e = G.edg;
    auto& L = G.prop.layer;
    int n = G.n, m = G.m, k = G.prop.k;
    VI newktruss(m);
    VI origin_attribute(G.A, 0);
    std::unordered_set<int> ktnod;
    for(int i = 0; i < m; i ++){
        if(G.prop.ktruss[i]){
            //计算origin_attribute
            ktnod.insert(e[i].first);
            ktnod.insert(e[i].second);
        }
    }
    for(auto it : ktnod){
        origin_attribute[G.attr[it]] ++;
    }
    for(int i = 0; i < G.A; i ++){
        origin_attribute[i] = std::min(origin_attribute[i], F);
    }

    int maxval = -1, argmaxval = -1, query_dist = INT_MAX;
    VI layerpoint, vertex_distance(n + 1, INT_MAX);
    std::fill(newktruss.begin(), newktruss.end(), 0);
    for(int i = 0; i < L.layer.size(); i ++){
        for(auto eid : L.layer[i]){
            layerpoint.push_back(e[eid].first);
            layerpoint.push_back(e[eid].second);
        }
    }
    std::sort(layerpoint.begin(), layerpoint.end(), [&](int x, int y){
        if(L.UB[x] == L.UB[y]){
            return x < y;
        }
        return L.UB[x] > L.UB[y];
    });
    int len = std::unique(layerpoint.begin(), layerpoint.end()) - layerpoint.begin();
    
    std::cerr << len << " candidate anchors\n";
    //预处理query_dist以及ktruss中最大的vertex_distance
    std::queue<int> Q;
    VI vis(n + 1);
    Q.push(q);
    vis[q] = 1;
    vertex_distance[q] = 0;
    while(!Q.empty()){
        int u = Q.front();
        Q.pop();
        for(auto eid : G.adj[u]){
            int v = e[eid].first ^ e[eid].second ^ u;
            if(!vis[v]){
                Q.push(v);
                vis[v] = 1;
                vertex_distance[v] = vertex_distance[u] + 1;
            }
        }
    }
    int qdktruss = 0;
    for(int i = 0; i < m; i ++){
        if(G.prop.ktruss[i]){
            qdktruss = std::max({qdktruss, vertex_distance[e[i].first],
                vertex_distance[e[i].second]});
        }
    }

    //开始逐个计算

    compute_anchor_init(G);

    for(int i = 0; i < len; i ++){
#ifdef COMPACTNESS
        if(L.UB[layerpoint[i]] == 0){
            break;
        }
#else
        if(L.UB[layerpoint[i]] < maxval || L.UB[layerpoint[i]] == 0){
            break;
        }
#endif
        // if(L.UB[layerpoint[i]] == maxval && query_dist == qdktruss){
        //     break;
        // }
#ifdef debug
        std::cerr << "candidate " << i << " : " << layerpoint[i] << 
            " UB " << L.UB[layerpoint[i]];
#endif

        //获得followers
        VI followers = compute_follower(G, L, q, F, layerpoint[i]);
        //计算val
        int valres = 0, qdres = qdktruss; //初始化为ktruss的qd值
        VI attrval(G.A + 1);
        for(auto v : followers){
            attrval[G.attr[v]] ++;
        }
        for(int j = 0; j < G.A; j ++){
            valres += std::min(attrval[j], F - origin_attribute[j]);
        }
        //计算query_dist最大值
        for(auto v : followers){
            qdres = std::max(qdres, vertex_distance[v]);
        }
        //比较
#ifdef COMPACTNESS
        if(query_dist > qdres && valres > 0){
            maxval = valres;
            query_dist = qdres;
            argmaxval = layerpoint[i];
        }else if(query_dist == qdres && valres > maxval){
            maxval = valres;
            argmaxval = layerpoint[i];
        }
#else
        if(valres > maxval){
            maxval = valres;
            query_dist = qdres;
            argmaxval = layerpoint[i];
        }else if(valres == maxval && query_dist > qdres){
            query_dist = qdres;
            argmaxval = layerpoint[i];
        }
#endif

#ifdef debug
        std::cerr << " val " << valres << " qd " << qdres 
            << " followers " << followers.size() << "\n";
#endif
    }

    if(maxval <= 0){
        return {VI(), -1};
    }

    //需要计算newktruss
    VI followers = compute_follower(G, L, q, F, argmaxval);
#ifdef debug
    std::cerr << "\n";
#endif
    VI mark(n + 1);
    for(int i = 0; i < m; i ++){
        if(G.prop.ktruss[i]){
            mark[e[i].first] = mark[e[i].second] = 1;
        }
    }
    for(auto fo : followers){
        mark[fo] = 1;
    }
    for(int i = 0; i < m; i ++){
        if(mark[e[i].first] && mark[e[i].second]){
            newktruss[i] = 1;
        }
    }
    std::fill(origin_attribute.begin(), origin_attribute.end(), 0);
    for(int i = 1; i <= n; i ++){
        if(mark[i]){
            origin_attribute[G.attr[i]] ++;
        }
    }
    return {newktruss, argmaxval};
}
