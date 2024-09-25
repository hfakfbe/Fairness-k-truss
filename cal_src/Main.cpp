#include "Truss.h"

std::tuple<VPII, VI, int> solve(Graph& G, int q, int F, int b){

    std::cerr << "decomposition\n";
    
    clock_t beg = clock(), end;
    G.prop.trussness = decomposition(G); // ac
    end = clock();
    std::cerr << end - beg << "us\n";
    
    
    // std::cerr << "compute_maxk\n";
    auto [G2, kmax] = compute_maxk(G, q, F);
    std::cerr << "G2 has " << G2.m << " edges\n";
    // kmax = 70;
    // Graph G2 = G;
    if(kmax == 2){
        std::cerr << "not exist\n";
        return {VPII(), VI(), 2};
    }

    for(int k = kmax; k > 2; k --){
        int ok = 0;
        VPII res;
        VI S;
    
        G2.prop.k = k;
        
        std::cerr << "compute_property k=" << k << "\n";
        G2.prop = compute_property(G2, q, F);

        int val = compute_value(G2, G2.prop.ktruss, F);
        std::cerr << "current val: " << val << "\n";
        
        if(val == F * G2.A){
            for(int i = 0; i < G2.m; i ++){
                if(G2.prop.ktruss[i]){
                    res.push_back(G2.edg[i]);
                }
            }
            return {res, VI(), k};
        }

        for(int i = 0; i < b; i ++){

            std::cerr << "compute_layer b=" << i << "\n";
            G2.prop.layer = compute_layer(G2, q, F);

            std::cerr << "compute_anchor\n";
            auto [T1, a] = compute_anchor(G2, q, F);

            if(T1.empty()){
                break;
            }

            S.push_back(a);

            // std::cerr << a << ": ";
            // for(int i = 0; i < G2.m; i ++){
            //     std::cerr << T1[i] << " \n"[i == G2.m - 1];
            // }

            if(compute_value(G2, T1, F) == F * G2.A){
                ok = 1;
                for(int j = 0; j < G2.m; j ++){
                    if(G2.prop.ktruss[j]){
                        res.push_back(G2.edg[j]);
                    }
                }
                G.prop.S = S;
                break;
            }

            std::cerr << "update_property\n";
            G2.prop = update_property(G2, T1, q, F);
        }
        if(ok){
            return {res, G2.prop.S, k};
        }
    }

    return {VPII(), VI(), 2};
}

int main(){
    Graph G = read_graph("../dblp_A4.txt");

    std::vector<std::tuple<int, int, int>> query = read_query("../dblpqry.txt");

    for(int i = 0; i < query.size(); i ++){
        auto [q, F, b] = query[i];

        clock_t beg = clock(), end;
        auto [res, S, k] = solve(G, q, F, b);
        end = clock();
        
        std::cerr << end - beg << "us\n";

        std::string str = std::to_string(i) + ".out";
        std::ofstream outfile(str);

        outfile << res.size() << " " << S.size() << " " << k << "\n";
        for(auto u : S){
            outfile << u << " ";
        }
        outfile << "\n";
        for(auto [u, v] : res){
            outfile << u << " " << v << "\n";
        }
        outfile.close();
    }

}