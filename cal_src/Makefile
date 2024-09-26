# 定义编译器
CXX = g++

# 定义默认编译选项
CXXFLAGS = -std=c++20 -O3

# 定义debug编译选项
CXXFLAGS_DEBUG = -std=c++20 -Ddebug -ggdb

# 定义源文件和目标文件
SOURCES = Anchor.cpp Decomposition.cpp Layer.cpp Maxk.cpp Property.cpp Read.cpp
OBJECTS = $(SOURCES:.cpp=.o)
MAIN_SOURCE = Main.cpp
MAIN_OBJECT = Main.o
TEST_SOURCE = Testmaxk.cpp
TEST_OBJECT = Testmaxk.o

# 定义目标程序
MAIN_TARGET = Main
TEST_TARGET = Testmaxk

# 默认目标
all: $(MAIN_TARGET)

test: $(TEST_TARGET)

# 编译所有源文件为目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 编译Main.cpp并链接成可执行文件
$(MAIN_TARGET): $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译TestL.cpp并链接成可执行文件
$(TEST_TARGET): $(TEST_OBJECT) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译debug版本
debug: CXXFLAGS := $(CXXFLAGS_DEBUG)
debug: $(MAIN_TARGET)

# 清理
clean:
	rm -f $(ALL_TARGETS) *.o

# 定义所有目标，以便在clean中使用
ALL_TARGETS = $(MAIN_TARGET) TestL Testmaxk

# 伪目标
.PHONY: all clean debug test