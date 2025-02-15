#include "Truss.h"
#include <mutex>
#include <thread>

int MAXN = 100000;

struct checkstr{
    int n, m, S, k, d;
    clock_t t;
};

std::mutex mtx;  // 互斥锁用于保护全局直径的更新
int global_max_d = 0;  // 全局最大直径

void bfs_diameter_range(int start_idx, int end_idx, const VI& vertices, const std::unordered_map<int, VI>& adj, const std::unordered_map<int, int>& p) {
    for(int i = start_idx; i < end_idx; ++i) {
        std::queue<int> Q;
        VI dis(vertices.size(), -1);
        Q.push(vertices[i]);
        dis[i] = 0;

        while(!Q.empty()){
            int u = Q.front();
            Q.pop();
            int pu = p.at(u);
            for(auto v : adj.at(u)){
                int pv = p.at(v);
                if(dis[pv] == -1){
                    dis[pv] = dis[pu] + 1;
                    Q.push(v);
                }
            }
        }

        int local_max_d = *std::max_element(dis.begin(), dis.end());

        // 使用互斥锁保护更新全局直径的部分
        std::lock_guard<std::mutex> lock(mtx);
        global_max_d = std::max(global_max_d, local_max_d);
    }
}

checkstr check_community(std::ifstream &fin, int q, int F){
    // m n S k
    // S anchors in a line
    // m lines
    // n vertices in a line
    int n, m, S, k;
    std::unordered_map<int, VI> adj;
    std::unordered_map<int, int> p;
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

    checkstr res;
    res.n = n;
    res.m = m;
    res.S = S;
    res.k = k;
    res.d = -1;
    res.t = -1;

    if(m == 0){
        return res;
    }else if(m < MAXN){
        // 多线程计算直径
        global_max_d = 0;
    
        // 确定合适的线程数，通常是系统硬件线程数
        int num_threads = std::min<int>({(int) std::thread::hardware_concurrency(), n, 16});
        int vertices_per_thread = n / num_threads;  // 每个线程处理的顶点数
        int remaining_vertices = n % num_threads;   // 处理不均分的顶点
    
        std::vector<std::thread> threads;
    
        int start_idx = 0;
        for(int t = 0; t < num_threads; ++t) {
            int end_idx = start_idx + vertices_per_thread + (t < remaining_vertices ? 1 : 0);  // 分配额外的顶点给前 remaining_vertices 个线程
            threads.emplace_back(bfs_diameter_range, start_idx, end_idx, std::cref(vertices), std::cref(adj), std::cref(p));
            start_idx = end_idx;
        }
    
        // 等待所有线程完成
        for(auto &t : threads){
            t.join();
        }
        res.d = global_max_d;
    }

    fin >> res.t; // time us
    return res;
}

int main(int argc, char *argv[]){
    if(argc == 4){
        MAXN = atoi(argv[3]);
    }else if(argc != 3){
        //用法
        std::cout << "Usage: ./check_community <query> <output_dir> [MAXN]\n";
        exit(1);
    }
    // 是否有解，是否爆了，直径多少
    std::vector<std::tuple<int, int, int, int>> query = read_query(argv[1]);
    std::string sf = argv[2];
    std::ofstream fout((sf + "/summary.out").c_str());
    for(int i = 0; i < query.size(); i ++){
        auto [q, F, R, b] = query[i];

        std::string str = sf + "/" + std::to_string(i) + ".out";
        
        if(!fs::exists(str)){
            break;
        }
        std::ifstream fin(str);
        auto cks = check_community(fin, q, F);
        fout << q << " " << F << " " << R << " " << b << " d: " << cks.d << " n: " << cks.n << " m: " << cks.m << " S: " << cks.S << " k: " << cks.k << " t: " << 1.0 * cks.t / (1e6) << "\n";
    }
}