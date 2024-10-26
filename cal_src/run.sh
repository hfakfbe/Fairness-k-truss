#!/bin/bash  

datas=("facebook" "dblp" "eucore" "LiveJournal" "orkut" "amazon")    

# 定义一个函数来处理单个数据集  
process_data() {  
    local data=$1  
    local input_file="../data/expqry/${data}_A4_processed.txt"  
    local data_file="../data/${data}_A4.txt"  
    local output_dir="../out/${data}"
    local addfb_file="${output_dir}/${data}.txt"  
    local output_log="${output_dir}/log.out"
  
    rm ${output_dir}/*
    
    if [ ! -f "$input_file" ]; then  
        echo "输入文件 $input_file 不存在"  
        exit 1  
    fi  

    if [ ! -d "$output_dir" ]; then  
        mkdir "$output_dir"  
    fi
  
    > ${addfb_file}
    while IFS= read -r line  
    do  
        echo "${line} 10 10" >> "${addfb_file}"  
    done < "${input_file}"  
    > ${output_log}

    # 正式开始
    set -x
    ../bin/Compact ${data_file} ${addfb_file} ${output_dir} 2> ${output_log}  
    if [ $? -ne 0 ]; then  
        echo "Error: ${data}"  
        exit 1  
    fi  
    ../bin/Test ${addfb_file} ${output_dir}
    set +x
}  

# 启动所有后台作业  
for data in "${datas[@]}"  
do  
    process_data "$data" &
done  
  
# 等待所有后台作业完成  
wait  
  
echo "所有任务已完成"