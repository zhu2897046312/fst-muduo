
aux_source_directory(./ SRC_LIST)
#muduo_net 依赖 muduo_base

#编译动态库
add_library(fst_muduo SHARED ${SRC_LIST})
