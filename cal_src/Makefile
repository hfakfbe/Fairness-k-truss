# 定义编译器
CXX = g++
# CXXFLAGS = -std=c++20 -O3
CXXFLAGS = -std=c++20 -Ddebug -ggdb

# 定义源文件和目标文件
SOURCES = Anchor.cpp Decomposition.cpp Layer.cpp Maxk.cpp Property.cpp Read.cpp
OBJECTS = $(SOURCES:.cpp=.o)
MAIN_SOURCE = Main.cpp
MAIN_OBJECT = Main.o
TEST_SOURCE = Testmaxk.cpp
TEST_OBJECT = Testmaxk.o

# 定义目标程序
FAIR_TARGET = Fair
COMPACT_TARGET = Compact
TEST_TARGET = Testmaxk
ALL_TARGETS = $(FAIR_TARGET) $(COMPACT_TARGET) TestL Testmaxk

# 指令
all: $(FAIR_TARGET)
compact: CXXFLAGS += -DCOMPACTNESS
compact: $(COMPACT_TARGET)
test: $(TEST_TARGET)
clean:
	rm -f $(ALL_TARGETS) *.o

# 编译所有源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 编译main并链接成可执行文件
$(FAIR_TARGET): $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(COMPACT_TARGET): $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(TEST_TARGET): $(TEST_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 伪目标
.PHONY: all compact clean test