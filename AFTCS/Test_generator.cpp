#include <bits/stdc++.h>

int main(int argc, char *argv[]){
    assert(argc >= 3);
    int n = atoi(argv[1]), m = atoi(argv[2]), a = 1;
    assert(1LL * n * (n - 1) / 2 >= m);
    if(argc >= 4){
        a = atoi(argv[3]);
    }

    std::mt19937 rng(std::chrono::system_clock().now().time_since_epoch().count());
    
    std::cout << n << " " << m << "\n";
    std::set<std::pair<int, int>> set;
    for(int i = 0; i < m; i ++){
        int u, v;
        do{
            u = rng() % n + 1;
            v = rng() % n + 1;
            if(u > v){
                std::swap(u, v);
            }
        }while(u == v || set.count({u, v}));
        set.insert({u, v});
        std::cout << u << " " << v << "\n";
    }
    for(int i = 0; i < n; i ++){
        std::cout << rng() % a << " ";
    }

    std::cerr << rng() % n + 1 << " " << rng() % std::min(n, int(std::sqrt(n) + 1)) << " " << n;
}