#include "Truss.h"

std::tuple<VPII, VI, int> solve(Graph& G2, int q, int F, int b){
    DEBUG("compute_maxk\n");
    int tau_q = 0;
    for(auto eid : G2.adj[q]){
        tau_q = std::max(tau_q, G2.prop.trussness[eid]);
    }
    int kmax = tau_q;

    if(kmax == 2){
        DEBUG("not exist\n");
        return {VPII(), VI(), 2};
    }

    for(int k = kmax; k > 2; k --){
        int ok = 0;
        VPII res;
        VI S;
    
        G2.prop.k = k;
        
        DEBUG(std::string("compute_property k=") + std::to_string(k) + "\n");
        G2.prop = compute_property(G2, q, F);

        int val = compute_value(G2, G2.prop.ktruss, F);
        DEBUG(std::string("current val: ") + std::to_string(val) + "\n");
        
        if(val == F * G2.A){
            for(int i = 0; i < G2.m; i ++){
                if(G2.prop.ktruss[i]){
                    res.push_back(G2.edg[i]);
                }
            }
            return {res, VI(), k};
        }

        for(int i = 0; i < b; i ++){

            DEBUG(std::string("compute_layer b=") + std::to_string(i) + "\n");
            G2.prop.layer = compute_layer(G2, q, F);

            DEBUG("compute_anchor\n");
            auto [T1, a] = compute_anchor(G2, q, F);

            if(T1.empty()){
                DEBUG("T1 empty");
                break;
            }

            S.push_back(a);

            int curval = compute_value(G2, T1, F);
            DEBUG(std::string("now the val is ") + std::to_string(curval) + "\n");
            if(curval == F * G2.A){
                ok = 1;
                for(int j = 0; j < G2.m; j ++){
                    if(G2.prop.ktruss[j] || T1[j]){
                        res.push_back(G2.edg[j]);
                    }
                }
                G2.prop.S = S;
                break;
            }

            DEBUG("update_property\n");
            G2.prop = update_property(G2, T1, q, F);
        }
        if(ok){
            return {res, G2.prop.S, k};
        }
    }

    return {VPII(), VI(), 2};
}

// data qry out trussness
int main(int argc, char *argv[]){
    if(argc != 5){
        std::cerr << "main: 4 arguments!\n";
        exit(1);
    }

    Graph G = read_graph(argv[1]);
    if(fs::exists(argv[4])){
        G.prop.trussness.resize(G.m); 
        std::ifstream tin(argv[4]);
        for(int i = 0; i < G.m; i ++){
            tin >> G.prop.trussness[i];
        }
    }else{
        G.prop.trussness = decomposition(G);
        std::ofstream tout(argv[4]);
        for(int i = 0; i < G.m; i ++){
            tout << G.prop.trussness[i] << " ";
        }
        tout.close();
    }

    std::vector<std::tuple<int, int, int, int>> query = read_query(argv[2]);

    for(int i = 0; i < query.size(); i ++){
        auto [q, F, R, b] = query[i];

        std::cerr << "--------------------------------------------------------------------------\n";
        std::cerr << "q: " << q << " F: " << F << " R: " << R << " b: " << b << "\n";

        std::string str = argv[3];
        str += "/" + std::to_string(i) + ".out";
        if(fs::exists(str)){
            continue;
        }

        if(R != G.A){
            G.A = R;
            srand(1);
            for(int j = 1; j <= G.n; j ++){
                G.attr[j] = rand() % R;
            }
        }

        clock_t beg = clock(), end;
        auto [res, S, k] = solve(G, q, F, b);
        end = clock();
        
        std::cerr << end - beg << "us\n";

        std::ofstream outfile(str);
        std::set<int> set;

        for(auto [u, v] : res){
            set.insert(u);
            set.insert(v);
        }
        outfile << res.size() << " " << set.size() << " " << S.size() << " " << k << "\n";
        for(auto u : S){
            outfile << u << " ";
        }
        outfile << "\n";
        for(auto [u, v] : res){
            outfile << u << " " << v << "\n";
        }
        for(auto v : set){
            outfile << v << " ";
        }
        outfile << "\n";
        if(k == 2){
            outfile << "-1\nNot Exist!";
        }else{
            outfile << end - beg;
        }
        outfile.close();
    }

}