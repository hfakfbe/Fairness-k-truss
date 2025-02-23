

# FTCS

Dataset format: The first line contains two numbers, representing the maximum node index $n$ and the number of edges $m$ respectively. The next $m$ lines each contain two numbers $u, v$, representing the endpoints of an edge (nodes are 1-indexed). The following line contains $n$ numbers, where the $i$-th number represents the attribute of node $i$ (attributes are 0-indexed).

Navigate to the `FTCS` folder, place the dataset at: `your_path/FTCS/Dataset/DataName/`, and name the dataset `DataName.txt`.

After completing the above steps, your dataset path (e.g. deezer) should be: `your_path/FTCS/Dataset/deezer/deezer.txt`

Compilation command: `g++ main.cpp -o main -std=c++11`

Execution format: `./main <dataset name> <theta> <q> <attribute range> <gamma>`

Run case study: `./main deezer 10 15921 2 1`

Run FTCS-Batch:
Examples:  
`./main DBLP 500 40000 2 5`  
`./main com-lj.ungraph 100000 101000 2 1000`