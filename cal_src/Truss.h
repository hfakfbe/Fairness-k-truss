/*
1. Truss Decomposition
    input G
    output tau(e)


2. Maximal anchored k-truss
    input G, q, F
    output k_max, G'

4. Layer and UB
    input G, q, F
    output Layer

5. Anchoring
    input G', q, F
    output S, C
*/

#ifndef debug
#pragma GCC optimize("Ofast")
#endif

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <string>
#include <cstring>
#include <map>
#include <functional>
#include <queue>
#include <stack>
#include <cassert>
#include <climits>

using i64 = long long;
using MII = std::unordered_map<int, int>;
using VMII = std::vector<MII>;
using VI = std::vector<int>;
using VVI = std::vector<VI>;
using PII = std::pair<int, int>;
using VPII = std::vector<PII>;
using VVPII = std::vector<VPII>;

struct Layer{
    VI UB; // edge
    VI layernum; // edge
    VVI layer; // edge
};

struct Property{
    VI trussness; // edge
    VI deg; // vertex
    VI ktruss; // edge, anchored
    VI ksubtruss; // edge
    VI ktrussnoq; // edge
    VI S; // anchors set
    Layer layer;
    int k;

    Property() 
    : trussness(), 
        deg(), 
        ktruss(), 
        ksubtruss(), 
        ktrussnoq(), 
        S(), 
        layer(), 
        k(0) {}  

    Property(const Property& other)  
    : trussness(other.trussness),  
        deg(other.deg),  
        ktruss(other.ktruss),  
        ksubtruss(other.ksubtruss),  
        ktrussnoq(other.ktrussnoq),  
        S(other.S),  
        layer(other.layer),  
        k(other.k) {}
};

struct Graph{
    VVI adj; // storage edge ids
    std::vector<PII> edg; // storage each edges
    VI attr; // vertex
    Property prop;
    int n, m, A; // count of vertices and edges

    Graph() 
    : adj(), 
        edg(), 
        attr(), 
        prop(), 
        n(0), 
        m(0), 
        A(0) {}  

    Graph(const Graph& other)  
    : adj(other.adj),  
        edg(other.edg),  
        attr(other.attr),  
        prop(other.prop),  
        n(other.n),  
        m(other.m),  
        A(other.A) {}
};

struct edge_link{
    const int maxn, maxm;
	struct data{
		int v, pre, next, index_in_e;
	};
    std::vector<data> e;
    std::vector<int> p;
	int eid; //分别是head[]和idx

	void insert(int u, int v, int index_in_e){
		e[eid].v = v;
		e[eid].next = p[u];
		e[eid].pre = -1;
		e[eid].index_in_e = index_in_e;
		if(p[u] != -1) e[p[u]].pre = eid;
		p[u] = eid++;
	}

	void erase(int cur_id){
		if(~e[cur_id].next)e[e[cur_id].next].pre = e[cur_id].pre;
		if(~e[cur_id].pre){
            e[e[cur_id].pre].next = e[cur_id].next;
        }
	}

	edge_link(int n, int m)
        : maxn(n + 1), maxm(m + 1), e(maxm * 2), p(maxn, -1), eid(0) {}
};

struct hash_map {
    const int maxn, maxm, mod;
    struct data {
        long long u;
        int v, next;
    };
    std::vector<data> e;
    std::vector<int> h;
    int cnt;

    int hash(long long u) { return u < mod ? u : u % mod; }

    int& operator[](long long u) {
        int hu = hash(u);
        for (int i = h[hu]; i; i = e[i].next){
            if (e[i].u == u) return e[i].v;
        }
        e[++ cnt] = (data){u, -1, h[hu]}, h[hu] = cnt;
        return e[cnt].v;
    }

    bool count(long long u){
        int hu = hash(u);
        for (int i = h[hu]; i; i = e[i].next){
            if (e[i].u == u) return true;
        }
        return false;
    }

    void erase(long long u){
        int hu = hash(u), pre;
        for (int i = h[hu]; i; i = e[i].next){
            if (e[i].u == u){
                if(i == h[hu]) h[hu] = e[i].next;
                else e[pre].next = e[i].next;
            }
            pre = i;
        }
    }

    hash_map(int n, int m)
        : maxn(n + 1), maxm(m + 1), cnt(0), e(maxm * 8), h(maxm * 4), mod(maxm * 4) {}
};

Graph read_graph(std::string file);

std::vector<std::tuple<int, int, int>> read_query(std::string file);

VI decomposition(const Graph& G);

std::pair<Graph, int> compute_maxk(const Graph& G, int q, int F);

Property compute_property(const Graph& G, int q, int F);

Property update_property(const Graph& G, VI& T, int q, int F);

int compute_value(const Graph& G, const VI& T, int F);

Layer compute_layer(const Graph& G, int q, int F);

VI compute_UB(const Graph& G, const Layer& L, int q, int F);

std::pair<VI, int> compute_anchor(const Graph& G, int q, int F);
