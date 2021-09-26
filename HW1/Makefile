CC := gcc#컴파일러 정의
CCFLAGS := -Wall -Werror -Wextra# 컴파일할때 쓸 flag들 정의
DBGFLAGS := -g# 디버그할 바이너리 생성할때 쓸 flag들 정의
CCOBJFLAGS := $(CCFLAGS) -c# 컴파일만 하고 목적 파일(.o) 생성할때 쓸 flag들 정의

# 경로 매크로 정의
BIN_PATH := bin# binary 파일 경로
OBJ_PATH := obj# 목적 파일(.o) 경로
SRC_PATH := src# 소스 파일(c일경우 .c) 경로
DBG_PATH := debug# 디버그할 binary 파일 경로

TARGET_NAME := HelloWorld
# 컴파일할 타겟 이름 매크로
ifeq ($(OS),Windows_NT) # 만약 현재 OS가 Windows 계열이면
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME)) # 타겟 이름에 .exe 붙이기
endif # 조건문 종료
TARGET := $(BIN_PATH)/$(TARGET_NAME)# binary 파일 이름에 경로 붙인 매크로
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)# debug binary 파일 이름에 경로 붙인 매크로

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c))) # SRC_PATH 각각에(foreach) .c 를 붙여서(addprefix) 해당 경로의 파일 리스트 SRC에 저장(wildcard)
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC))))) # SRC에서 확장자 떼고(basename) 디렉토리 떼고(notdir) .o를 뒤에 붙이고(addsuffix) 목적파일 경로를 붙여서(addprefix) OBJ에 저장
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC))))) # SRC에서 확장자 뗴고(basename) 디렉토리 떼고(notdir) .o를 뒤에 붙이고(addsuffix) 디버그 파일 경로를 붙여(addprefix) OBJ_DEBUG 에 저장

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG) # 목적파일들 삭제할 clean list 매크로 
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST) # make 과정에 생긴 파일들 전부 삭제할 clean list 매크로

# default 규칙
default: makedir all # 경로 생성 후 all 실행 

# non-phony targets - phony 없어도 되는 타겟들(파일 이름/타겟 이름으로 되는 규칙들)
$(TARGET): $(OBJ) # Target은 OBJ에 Dependent
	$(CC) $(CCFLAGS) -o $@ $(OBJ)
# TARGET 이름으로 binary 만드는 컴파일(링크) 규칙

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c # OBJ_PATH의 각 목적 파일들은 SRC_PATH의 각 소스 파일을 prerequisite로 가짐
	$(CC) $(CCOBJFLAGS) -o $@ $<
# OBJ_PATH/%.o 이름으로 SRC_PATH/%.c 각각 컴파일하는 규칙

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c # DBG_PATH의 각 목적 파일들은 SRC_PATH의 각 소스 파일들을 prerequisite로 가짐
	$(CC) $(CCOBJFLAGS) $(DBGFLAGS) -o $@ $<
# DBG_PATH/%.o 이름으로 SRC_PATH/%.c 각각 컴파일하는 규칙 - DBGFLAG 추가해서 컴파일

$(TARGET_DEBUG): $(OBJ_DEBUG) # TARGET_DEBUG는 OBJ_DEBUG들을 prerequisite로 가짐
	$(CC) $(CCFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@
# TARGET_DEBUG 이름으로 debug binary 만드는 컴파일(링크) 규칙

# phony rules - phony 규칙들은 해당 디렉토리에 파일이 있어도 만족하지 않은 상황으로 인지해, 실행됨.
.PHONY: makedir # makedir 타겟은 그 이름으로 된 파일이 있어도 실행
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH) # @가 붙으면 쉘에서 실행 - 필요한 폴더들 생성

.PHONY: all # all 타겟은 그 이름으로 된 파일이 있어도 실행
all: $(TARGET) # TARGET 규칙 prerequisite로 가짐 -> 타겟 생성

.PHONY: debug # debug 타겟은 그 이름으로 된 파일이 있어도 실행
debug: $(TARGET_DEBUG) # TARGET_DEBUG 규칙 prerequistie로 가짐 -> TARGET_DEBUG 생성

.PHONY: clean # clean 타겟은 그 이름으로 된 파일이 있어도 실행
clean:
	@echo CLEAN $(CLEAN_LIST) # clean할 리스트 쉘에 출력
	@rm -f $(CLEAN_LIST) # clean 할 파일들 삭제

.PHONY: distclean # distclean 타겟은 그 이름으로 된 파일이 있어도 실행
distclean:
	@echo CLEAN $(DISTCLEAN_LIST) # clean 할 리스트 쉘에 출력
	@rm -f $(DISTCLEAN_LIST) # clean 할 파일들 삭제
