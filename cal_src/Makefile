# 定义编译器
CXX = g++
CXXFLAGS = -std=c++20 -O3
# CXXFLAGS = -std=c++20 -Ddebug -ggdb

# 定义源文件和目标文件
SOURCES = Decomposition.cpp Maxk.cpp Property.cpp Read.cpp
OBJECTS = $(SOURCES:.cpp=.o)

ANCHOR_SOURCE = Anchor.cpp
ANCHOR_F_OBJECT = Anchor_f.o
ANCHOR_C_OBJECT = Anchor_c.o

LAYER_SOURCE = Layer.cpp
LAYER_NOUB_OBJECT = Layer_n.o
LAYER_UB_OBJECT = Layer_y.o

MAIN_SOURCE = Main.cpp
MAIN_OBJECT = Main.o

CHECK_SOURCE = Test_checkcom.cpp
CHECK_OBJECT = Test_checkcom.o

# 定义目标程序
FAIR_TARGET = ../bin/Fair
FAIR_N_TARGET = ../bin/Fair_NOUB
COMPACT_TARGET = ../bin/Compact
CHECK_TARGET = ../bin/Check
ALL_TARGETS = $(FAIR_TARGET) $(FAIR_N_TARGET) $(COMPACT_TARGET) $(CHECK_TARGET)

# 指令
all: $(ALL_TARGETS)
clean:
	rm -f *.o ../bin/*

# 编译所有源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(ANCHOR_F_OBJECT): $(ANCHOR_SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(ANCHOR_C_OBJECT): $(ANCHOR_SOURCE)
	$(CXX) $(CXXFLAGS) -DCOMPACTNESS -c -o $@ $<
$(LAYER_UB_OBJECT): $(LAYER_SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(LAYER_NOUB_OBJECT): $(LAYER_SOURCE)
	$(CXX) $(CXXFLAGS) -DNOUB -c -o $@ $<

# 编译main并链接成可执行文件
$(FAIR_TARGET): $(ANCHOR_F_OBJECT) $(OBJECTS) $(MAIN_OBJECT) $(LAYER_UB_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(COMPACT_TARGET): $(ANCHOR_C_OBJECT) $(OBJECTS) $(MAIN_OBJECT) $(LAYER_NOUB_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(FAIR_N_TARGET): $(ANCHOR_F_OBJECT) $(OBJECTS) $(MAIN_OBJECT) $(LAYER_NOUB_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(CHECK_TARGET): $(ANCHOR_F_OBJECT) $(OBJECTS) $(CHECK_OBJECT) $(LAYER_UB_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 伪目标
.PHONY: all clean