ROOT_DIR = $(shell pwd)
DIR_LIB ?= $(ROOT_DIR)/lib

CC ?= gcc
CXX ?= g++
AR ?= ar
MOC = moc


INC := -I ./moc \
	-I /usr/include/x86_64-linux-gnu/qt5 

LDFLAGS = -L /usr/lib/x86_64-linux-gnu/qt5 \
	-L./lib \
	-lQt5Widgets	\
	-lQt5Gui	\
	-lQt5Core

CFLAGS = -DQT_STRICT_ITERATORS \
	-Wall -fPIC -lpthread -lrt -lstdc++ -std=c++11 -ldl -O0 -g

SRC_DIR += \
./		\

MOC_DIR := ./moc
DIR_OBJ := ./obj
TARGET := a.out


.PHONY: all CHECKDIR mocs clean 

all: CHECKDIR $(TARGET)


HEADERS += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.h))
CPP_SRCS += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))

CPP_OBJS := $(patsubst %.cpp, %.o, $(CPP_SRCS))
CPPOBJS := $(addprefix $(DIR_OBJ)/, $(CPP_OBJS))

# 自需要 moc 的文件
MOC_SOURCES = $(filter-out main.cpp, $(notdir $(CPP_SRCS)))
MOC_HEADERS = $(filter %.h, $(HEADERS))

# 生成 moc 文件（.cpp 和 .h 都要处理）
MOC_H_OBJS = $(addprefix $(MOC_DIR)/, $(patsubst %.h, moc_%_h.cpp, $(notdir $(MOC_HEADERS))))
MOC_CPP_OBJS =  $(addprefix $(MOC_DIR)/, $(patsubst %.cpp, moc_%.cpp, $(notdir $(MOC_SOURCES))))
MOCCPPOBJS =  $(addprefix $(DIR_OBJ)/, $(patsubst %.cpp, moc_%.o, $(notdir $(MOC_SOURCES))))
MOCCPPOBJS +=  $(addprefix $(DIR_OBJ)/, $(patsubst %.h, moc_%_h.o, $(notdir $(MOC_HEADERS))))

# moc 处理规则
$(MOC_H_OBJS): $(MOC_DIR)/moc_%_h.cpp: %.h
	@echo "MOC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(MOC) $< -o $@

$(MOC_CPP_OBJS): $(MOC_DIR)/moc_%.cpp: %.cpp
	@echo "MOC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(MOC) $< -o $@

$(MOCCPPOBJS): $(DIR_OBJ)/%.o: $(MOC_DIR)/%.cpp
	@echo "CC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(CXX) $(CFLAGS) -o $@ -c $< $(INC)

$(CPPOBJS): $(DIR_OBJ)/%.o: %.cpp
	@echo "CC $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(CXX) $(CFLAGS)  -o $@ -c $< $(INC)


$(TARGET): $(MOCCPPOBJS) $(CPPOBJS)
	$(CC) $(CPPOBJS) $(MOCCPPOBJS) -g -o $@ $(INC) $(LDFLAGS) $(CFLAGS) 

mocs: $(MOC_H_OBJS) $(MOC_CPP_OBJS) 

debug:
	echo "MOC_H_OBJS" $(MOC_H_OBJS)
	echo "CPPOBJS" $(CPPOBJS)
	echo "MOCCPPOBJS" $(MOCCPPOBJS)
	echo "MOC_CPP_OBJS" $(MOC_CPP_OBJS)
	echo "MOC_H_OBJS" $(MOC_H_OBJS)

CHECKDIR:
	@mkdir -p $(DIR_OBJ)
	@mkdir -p $(MOC_DIR)

clean:
	@rm -rf ${DIR_OBJ}
	@rm -rf ${MOC_DIR}
ifeq ($(TARGET), $(wildcard $(TARGET)))
	@rm $(TARGET)
endif
