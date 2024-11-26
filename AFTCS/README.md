# AFTCS Source File

This project contains implementations of graph algorithms with different strategies. After running `make`, four executable files are generated. Below is the usage guide for each executable.  

## Executables and Usage  

1. **Fair Strategy**  
   ```bash
   ./Fair data.txt query.txt outputdir trussness.txt
   ```  

2. **Fair Strategy Without UB (NOUB)**  
   ```bash
   ./Fair_NOUB data.txt query.txt outputdir trussness.txt
   ```  

3. **Compact Strategy**  
   ```bash
   ./Compact data.txt query.txt outputdir trussness.txt
   ```  

4. **Query Verification**  
   ```bash
   ./Check query.txt outputdir
   ```  

## Input Files  

1. **`data.txt`**  
   - This is the data file.  
   - Format:  
     - The first line contains two integers, `n` and `m`, where `n` is the number of nodes and `m` is the number of edges.  
     - The next `m` lines each contain two integers, `u` and `v`, representing an edge between nodes `u` and `v`.  
     - The last line contains `n` integers, representing the default attributes of the nodes.  

2. **`query.txt`**  
   - This is the query file.  
   - Format: Each line represents one query and contains four integers in the following order:  
     - `q`: Query node ID  
     - `F`: Query parameter  
     - `R`: Query parameter  
     - `b`: Query parameter  

3. **`trussness.txt`**  
   - This file contains the truss decomposition results.  
   - If the file does not already exist in the specified directory, the program will automatically create it.  

## Output  

The output will be stored in the directory specified by `outputdir`. Make sure this directory exists or has appropriate write permissions before running the programs.  

## Notes  

- Ensure the input files follow the specified formats to avoid errors.  
- The `outputdir` parameter must point to a valid directory where the results will be written.  
