# 定义编译器
CXX = g++
CXXFLAGS = -std=c++20 -O3
# CXXFLAGS = -std=c++20 -Ddebug -ggdb

# 定义源文件和目标文件
SOURCES = Decomposition.cpp Layer.cpp Maxk.cpp Property.cpp Read.cpp Update.cpp
OBJECTS = $(SOURCES:.cpp=.o)

ANCHOR_SOURCE = Anchor.cpp
ANCHOR_F_OBJECT = Anchor_f.o
ANCHOR_C_OBJECT = Anchor_c.o

MAIN_SOURCE = Main.cpp
MAIN_OBJECT = Main.o

TEST_SOURCE = Test_update.cpp
TEST_OBJECT = Test_update.o

CHECK_SOURCE = Test_checkcom.cpp
CHECK_OBJECT = Test_checkcom.o

# 定义目标程序
FAIR_TARGET = ../bin/Fair
COMPACT_TARGET = ../bin/Compact
TEST_TARGET = ../bin/Test
CHECK_TARGET = ../bin/Check
ALL_TARGETS = $(FAIR_TARGET) $(COMPACT_TARGET) $(TEST_TARGET)

# 指令
all: $(FAIR_TARGET) $(COMPACT_TARGET)
test: $(TEST_TARGET)
check: $(CHECK_TARGET)
clean:
	rm -f *.o

# 编译所有源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(ANCHOR_F_OBJECT): $(ANCHOR_SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(ANCHOR_C_OBJECT): $(ANCHOR_SOURCE)
	$(CXX) $(CXXFLAGS) -DCOMPACTNESS -c -o $@ $<

# 编译main并链接成可执行文件
$(FAIR_TARGET): $(ANCHOR_F_OBJECT) $(OBJECTS) $(MAIN_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(COMPACT_TARGET): $(ANCHOR_C_OBJECT) $(OBJECTS) $(MAIN_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(TEST_TARGET): $(TEST_OBJECT) $(OBJECTS) $(ANCHOR_F_OBJECT)
	$(CXX) $(CXXFLAGS) -o $@ $^
$(CHECK_TARGET): $(CHECK_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 伪目标
.PHONY: all clean test check