#include "Truss.h"

constexpr int MAXN = 100;

struct checkstr{
    int n, m, S, k, d;
};

checkstr check_community(std::ifstream &fin, int q, int F){
    // m n S k
    // S anchors in a line
    // m lines
    // n vertices in a line
    int n, m, S, k;
    std::map<int, VI> adj;
    std::map<int, int> p;
    fin >> m >> n >> S >> k;
    VI anchors(S), vertices(n);

    for(int i = 0; i < S; i ++){
        fin >> anchors[i];
    }
    for(int i = 0; i < m; i ++){
        int u, v;
        fin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    // std::cerr << m << " " << vertices.size() << " " << S << " " << k << "?";
    for(int i = 0; i < n; i ++){
        // std::cerr << "!";
        fin >> vertices[i];
        p[vertices[i]] = i;
    }
    if(m == 0 || n > MAXN){
        checkstr res;
        res.d = -1;
        return res;
    }
    int d = 0;
    for(int i = 0; i < n; i ++){
        std::queue<int> Q;
        VI dis(n, -1);
        Q.push(vertices[i]);
        dis[i] = 0;
        while(!Q.empty()){
            int u = Q.front();
            Q.pop();
            int pu = p[u];
            for(auto v : adj[u]){
                int pv = p[v];
                if(dis[pv] == -1){
                    dis[pv] = dis[pu] + 1;
                    Q.push(v);
                }
            }
        }
        d = std::max(d, *std::max_element(dis.begin(), dis.end()));
    }
    checkstr res;
    res.d = d;
    res.n = n;
    res.m = m;
    res.S = S;
    res.k = k;
    return res;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cerr << "main: 2 argument!\n";
        exit(1);
    }
    // 是否有解，是否爆了，直径多少
    std::vector<std::tuple<int, int, int>> query = read_query(argv[1]);
    std::string sf = argv[2];
    std::ofstream fout((sf + "/summary.out").c_str());
    for(int i = 0; i < query.size(); i ++){
        auto [q, F, b] = query[i];

        std::string str = sf + "/" + std::to_string(i) + ".out";
        std::ifstream fin(str);
        auto cks = check_community(fin, q, F);
        if(cks.d != -1){
            fout << q << " " << F << " " << b << " d: " << cks.d << " n: " << cks.n << " m: " << cks.m << " S: " << cks.S << " k: " << cks.k << "\n";
        }
    }
}