# stripper_qmm - QMM Plugin
# Copyright 2026
# 3-clause BSD license: https://opensource.org/license/bsd-3-clause

BIN_32 := stripper_qmm
BIN_64 := stripper_qmm_x86_64

CC := g++

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=%.o)

CPPFLAGS := -MMD -MP -I ./include -isystem ../qmm_sdks -isystem ../qmm2/include
CFLAGS   := -Wall -pipe -fPIC
LDFLAGS  := -shared -fPIC
LDLIBS   :=

REL_CPPFLAGS := $(CPPFLAGS) -DNDEBUG
DBG_CPPFLAGS := $(CPPFLAGS) -D_DEBUG

REL_CFLAGS_32 := $(CFLAGS) -m32 -O2 -falign-functions=16 -fno-strict-aliasing -fstrength-reduce 
REL_CFLAGS_64 := $(CFLAGS) -O2 -falign-functions=16 -fno-strict-aliasing -fstrength-reduce 
DBG_CFLAGS_32 := $(CFLAGS) -m32 -g -pg
DBG_CFLAGS_64 := $(CFLAGS) -g -pg

REL_LDFLAGS_32 := $(LDFLAGS) -m32
REL_LDFLAGS_64 := $(LDFLAGS)
DBG_LDFLAGS_32 := $(LDFLAGS) -m32 -g -pg
DBG_LDFLAGS_64 := $(LDFLAGS) -g -pg

.PHONY: help all clean release debug release32 debug32 release64 debug64 $(addprefix game-,$(GAMES)) $(addprefix release-,$(GAMES)) $(addprefix debug-,$(GAMES))

help:
	@echo make targets:
	@echo all: release debug
	@echo release: release32 release64
	@echo release32: [32-bit release builds]
	@echo release64: [64-bit release builds]
	@echo debug: debug32 debug64
	@echo debug32: [32-bit debug builds]
	@echo debug64: [64-bit debug builds]
	@echo game-[GAME]: release-[GAME] debug-[GAME]
	@echo release-[GAME]: release32-[GAME] release64-[GAME]
	@echo debug-[GAME]: debug32-[GAME] debug64-[GAME]
	@echo release32-[GAME]: [32-bit release build for GAME]
	@echo release64-[GAME]: [64-bit release build for GAME]
	@echo debug32-[GAME]: [32-bit debug build for GAME]
	@echo debug64-[GAME]: [64-bit release build for GAME]

all: release debug
release: release32 release64
release32: release32-COD11MP release32-CODMP release32-CODUOMP release32-JAMP release32-JASP release32-JK2MP release32-JK2SP release32-KINGPIN release32-MOHAA release32-MOHBT release32-MOHSH release32-Q3A release32-QUAKE2 release32-RTCWMP release32-RTCWSP release32-SIN release32-SOF release32-SOF2MP release32-SOF2SP release32-STEF2 release32-STVOYHM release32-STVOYSP release32-WET
release64: release64-JAMP release64-JASP release64-JK2MP release64-JK2SP release64-Q3A release64-QUAKE2 release64-RTCWMP release64-RTCWSP release64-WET
debug: debug32 debug64
debug32: debug32-COD11MP debug32-CODMP debug32-CODUOMP debug32-JAMP debug32-JASP debug32-JK2MP debug32-JK2SP debug32-KINGPIN debug32-MOHAA debug32-MOHBT debug32-MOHSH debug32-Q3A debug32-QUAKE2 debug32-RTCWMP debug32-RTCWSP debug32-SIN debug32-SOF debug32-SOF2MP debug32-SOF2SP debug32-STEF2 debug32-STVOYHM debug32-STVOYSP debug32-WET
debug64: debug64-JAMP debug64-JASP debug64-JK2MP debug64-JK2SP debug64-Q3A debug64-QUAKE2 debug64-RTCWMP debug64-RTCWSP debug64-WET
game-COD11MP: release-COD11MP debug-COD11MP
release-COD11MP: release32-COD11MP
debug-COD11MP: debug32-COD11MP
release32-COD11MP: $(BIN_DIR)/release-COD11MP/x86/$(BIN_32)_COD11MP.so
debug32-COD11MP: $(BIN_DIR)/debug-COD11MP/x86/$(BIN_32)_COD11MP.so

$(BIN_DIR)/release-COD11MP/x86/$(BIN_32)_COD11MP.so: $(addprefix $(OBJ_DIR)/release-COD11MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-COD11MP/x86/$(BIN_32)_COD11MP.so: $(addprefix $(OBJ_DIR)/debug-COD11MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-COD11MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_COD11MP -DGAME_STR=\"COD11MP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-COD11MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_COD11MP -DGAME_STR=\"COD11MP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-COD11MP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-COD11MP/x86/,$(OBJ_FILES:.o=.d))
game-CODMP: release-CODMP debug-CODMP
release-CODMP: release32-CODMP
debug-CODMP: debug32-CODMP
release32-CODMP: $(BIN_DIR)/release-CODMP/x86/$(BIN_32)_CODMP.so
debug32-CODMP: $(BIN_DIR)/debug-CODMP/x86/$(BIN_32)_CODMP.so

$(BIN_DIR)/release-CODMP/x86/$(BIN_32)_CODMP.so: $(addprefix $(OBJ_DIR)/release-CODMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-CODMP/x86/$(BIN_32)_CODMP.so: $(addprefix $(OBJ_DIR)/debug-CODMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-CODMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_CODMP -DGAME_STR=\"CODMP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-CODMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_CODMP -DGAME_STR=\"CODMP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-CODMP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-CODMP/x86/,$(OBJ_FILES:.o=.d))
game-CODUOMP: release-CODUOMP debug-CODUOMP
release-CODUOMP: release32-CODUOMP
debug-CODUOMP: debug32-CODUOMP
release32-CODUOMP: $(BIN_DIR)/release-CODUOMP/x86/$(BIN_32)_CODUOMP.so
debug32-CODUOMP: $(BIN_DIR)/debug-CODUOMP/x86/$(BIN_32)_CODUOMP.so

$(BIN_DIR)/release-CODUOMP/x86/$(BIN_32)_CODUOMP.so: $(addprefix $(OBJ_DIR)/release-CODUOMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-CODUOMP/x86/$(BIN_32)_CODUOMP.so: $(addprefix $(OBJ_DIR)/debug-CODUOMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-CODUOMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_CODUOMP -DGAME_STR=\"CODUOMP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-CODUOMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_CODUOMP -DGAME_STR=\"CODUOMP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-CODUOMP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-CODUOMP/x86/,$(OBJ_FILES:.o=.d))
game-JAMP: release-JAMP debug-JAMP
release-JAMP: release32-JAMP release64-JAMP
debug-JAMP: debug32-JAMP debug64-JAMP
release32-JAMP: $(BIN_DIR)/release-JAMP/x86/$(BIN_32)_JAMP.so
debug32-JAMP: $(BIN_DIR)/debug-JAMP/x86/$(BIN_32)_JAMP.so

$(BIN_DIR)/release-JAMP/x86/$(BIN_32)_JAMP.so: $(addprefix $(OBJ_DIR)/release-JAMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-JAMP/x86/$(BIN_32)_JAMP.so: $(addprefix $(OBJ_DIR)/debug-JAMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JAMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JAMP -DGAME_STR=\"JAMP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-JAMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JAMP -DGAME_STR=\"JAMP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JAMP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JAMP/x86/,$(OBJ_FILES:.o=.d))
release64-JAMP: $(BIN_DIR)/release-JAMP/x86_64/$(BIN_64)_JAMP.so
debug64-JAMP: $(BIN_DIR)/debug-JAMP/x86_64/$(BIN_64)_JAMP.so

$(BIN_DIR)/release-JAMP/x86_64/$(BIN_64)_JAMP.so: $(addprefix $(OBJ_DIR)/release-JAMP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-JAMP/x86_64/$(BIN_64)_JAMP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JAMP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JAMP -DGAME_STR=\"JAMP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-JAMP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JAMP -DGAME_STR=\"JAMP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JAMP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JAMP/x86_64/,$(OBJ_FILES:.o=.d))
game-JASP: release-JASP debug-JASP
release-JASP: release32-JASP release64-JASP
debug-JASP: debug32-JASP debug64-JASP
release32-JASP: $(BIN_DIR)/release-JASP/x86/$(BIN_32)_JASP.so
debug32-JASP: $(BIN_DIR)/debug-JASP/x86/$(BIN_32)_JASP.so

$(BIN_DIR)/release-JASP/x86/$(BIN_32)_JASP.so: $(addprefix $(OBJ_DIR)/release-JASP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-JASP/x86/$(BIN_32)_JASP.so: $(addprefix $(OBJ_DIR)/debug-JASP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JASP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JASP -DGAME_STR=\"JASP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-JASP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JASP -DGAME_STR=\"JASP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JASP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JASP/x86/,$(OBJ_FILES:.o=.d))
release64-JASP: $(BIN_DIR)/release-JASP/x86_64/$(BIN_64)_JASP.so
debug64-JASP: $(BIN_DIR)/debug-JASP/x86_64/$(BIN_64)_JASP.so

$(BIN_DIR)/release-JASP/x86_64/$(BIN_64)_JASP.so: $(addprefix $(OBJ_DIR)/release-JASP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-JASP/x86_64/$(BIN_64)_JASP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JASP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JASP -DGAME_STR=\"JASP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-JASP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JASP -DGAME_STR=\"JASP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JASP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JASP/x86_64/,$(OBJ_FILES:.o=.d))
game-JK2MP: release-JK2MP debug-JK2MP
release-JK2MP: release32-JK2MP release64-JK2MP
debug-JK2MP: debug32-JK2MP debug64-JK2MP
release32-JK2MP: $(BIN_DIR)/release-JK2MP/x86/$(BIN_32)_JK2MP.so
debug32-JK2MP: $(BIN_DIR)/debug-JK2MP/x86/$(BIN_32)_JK2MP.so

$(BIN_DIR)/release-JK2MP/x86/$(BIN_32)_JK2MP.so: $(addprefix $(OBJ_DIR)/release-JK2MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-JK2MP/x86/$(BIN_32)_JK2MP.so: $(addprefix $(OBJ_DIR)/debug-JK2MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JK2MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JK2MP -DGAME_STR=\"JK2MP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-JK2MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JK2MP -DGAME_STR=\"JK2MP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JK2MP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JK2MP/x86/,$(OBJ_FILES:.o=.d))
release64-JK2MP: $(BIN_DIR)/release-JK2MP/x86_64/$(BIN_64)_JK2MP.so
debug64-JK2MP: $(BIN_DIR)/debug-JK2MP/x86_64/$(BIN_64)_JK2MP.so

$(BIN_DIR)/release-JK2MP/x86_64/$(BIN_64)_JK2MP.so: $(addprefix $(OBJ_DIR)/release-JK2MP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-JK2MP/x86_64/$(BIN_64)_JK2MP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JK2MP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JK2MP -DGAME_STR=\"JK2MP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-JK2MP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JK2MP -DGAME_STR=\"JK2MP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JK2MP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JK2MP/x86_64/,$(OBJ_FILES:.o=.d))
game-JK2SP: release-JK2SP debug-JK2SP
release-JK2SP: release32-JK2SP release64-JK2SP
debug-JK2SP: debug32-JK2SP debug64-JK2SP
release32-JK2SP: $(BIN_DIR)/release-JK2SP/x86/$(BIN_32)_JK2SP.so
debug32-JK2SP: $(BIN_DIR)/debug-JK2SP/x86/$(BIN_32)_JK2SP.so

$(BIN_DIR)/release-JK2SP/x86/$(BIN_32)_JK2SP.so: $(addprefix $(OBJ_DIR)/release-JK2SP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-JK2SP/x86/$(BIN_32)_JK2SP.so: $(addprefix $(OBJ_DIR)/debug-JK2SP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JK2SP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JK2SP -DGAME_STR=\"JK2SP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-JK2SP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JK2SP -DGAME_STR=\"JK2SP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JK2SP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JK2SP/x86/,$(OBJ_FILES:.o=.d))
release64-JK2SP: $(BIN_DIR)/release-JK2SP/x86_64/$(BIN_64)_JK2SP.so
debug64-JK2SP: $(BIN_DIR)/debug-JK2SP/x86_64/$(BIN_64)_JK2SP.so

$(BIN_DIR)/release-JK2SP/x86_64/$(BIN_64)_JK2SP.so: $(addprefix $(OBJ_DIR)/release-JK2SP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-JK2SP/x86_64/$(BIN_64)_JK2SP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-JK2SP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_JK2SP -DGAME_STR=\"JK2SP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-JK2SP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_JK2SP -DGAME_STR=\"JK2SP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-JK2SP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-JK2SP/x86_64/,$(OBJ_FILES:.o=.d))
game-KINGPIN: release-KINGPIN debug-KINGPIN
release-KINGPIN: release32-KINGPIN
debug-KINGPIN: debug32-KINGPIN
release32-KINGPIN: $(BIN_DIR)/release-KINGPIN/x86/$(BIN_32)_KINGPIN.so
debug32-KINGPIN: $(BIN_DIR)/debug-KINGPIN/x86/$(BIN_32)_KINGPIN.so

$(BIN_DIR)/release-KINGPIN/x86/$(BIN_32)_KINGPIN.so: $(addprefix $(OBJ_DIR)/release-KINGPIN/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-KINGPIN/x86/$(BIN_32)_KINGPIN.so: $(addprefix $(OBJ_DIR)/debug-KINGPIN/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-KINGPIN/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_KINGPIN -DGAME_STR=\"KINGPIN\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-KINGPIN/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_KINGPIN -DGAME_STR=\"KINGPIN\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-KINGPIN/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-KINGPIN/x86/,$(OBJ_FILES:.o=.d))
game-MOHAA: release-MOHAA debug-MOHAA
release-MOHAA: release32-MOHAA
debug-MOHAA: debug32-MOHAA
release32-MOHAA: $(BIN_DIR)/release-MOHAA/x86/$(BIN_32)_MOHAA.so
debug32-MOHAA: $(BIN_DIR)/debug-MOHAA/x86/$(BIN_32)_MOHAA.so

$(BIN_DIR)/release-MOHAA/x86/$(BIN_32)_MOHAA.so: $(addprefix $(OBJ_DIR)/release-MOHAA/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-MOHAA/x86/$(BIN_32)_MOHAA.so: $(addprefix $(OBJ_DIR)/debug-MOHAA/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-MOHAA/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_MOHAA -DGAME_STR=\"MOHAA\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-MOHAA/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_MOHAA -DGAME_STR=\"MOHAA\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-MOHAA/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-MOHAA/x86/,$(OBJ_FILES:.o=.d))
game-MOHBT: release-MOHBT debug-MOHBT
release-MOHBT: release32-MOHBT
debug-MOHBT: debug32-MOHBT
release32-MOHBT: $(BIN_DIR)/release-MOHBT/x86/$(BIN_32)_MOHBT.so
debug32-MOHBT: $(BIN_DIR)/debug-MOHBT/x86/$(BIN_32)_MOHBT.so

$(BIN_DIR)/release-MOHBT/x86/$(BIN_32)_MOHBT.so: $(addprefix $(OBJ_DIR)/release-MOHBT/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-MOHBT/x86/$(BIN_32)_MOHBT.so: $(addprefix $(OBJ_DIR)/debug-MOHBT/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-MOHBT/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_MOHBT -DGAME_STR=\"MOHBT\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-MOHBT/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_MOHBT -DGAME_STR=\"MOHBT\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-MOHBT/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-MOHBT/x86/,$(OBJ_FILES:.o=.d))
game-MOHSH: release-MOHSH debug-MOHSH
release-MOHSH: release32-MOHSH
debug-MOHSH: debug32-MOHSH
release32-MOHSH: $(BIN_DIR)/release-MOHSH/x86/$(BIN_32)_MOHSH.so
debug32-MOHSH: $(BIN_DIR)/debug-MOHSH/x86/$(BIN_32)_MOHSH.so

$(BIN_DIR)/release-MOHSH/x86/$(BIN_32)_MOHSH.so: $(addprefix $(OBJ_DIR)/release-MOHSH/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-MOHSH/x86/$(BIN_32)_MOHSH.so: $(addprefix $(OBJ_DIR)/debug-MOHSH/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-MOHSH/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_MOHSH -DGAME_STR=\"MOHSH\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-MOHSH/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_MOHSH -DGAME_STR=\"MOHSH\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-MOHSH/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-MOHSH/x86/,$(OBJ_FILES:.o=.d))
game-Q3A: release-Q3A debug-Q3A
release-Q3A: release32-Q3A release64-Q3A
debug-Q3A: debug32-Q3A debug64-Q3A
release32-Q3A: $(BIN_DIR)/release-Q3A/x86/$(BIN_32)_Q3A.so
debug32-Q3A: $(BIN_DIR)/debug-Q3A/x86/$(BIN_32)_Q3A.so

$(BIN_DIR)/release-Q3A/x86/$(BIN_32)_Q3A.so: $(addprefix $(OBJ_DIR)/release-Q3A/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-Q3A/x86/$(BIN_32)_Q3A.so: $(addprefix $(OBJ_DIR)/debug-Q3A/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-Q3A/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_Q3A -DGAME_STR=\"Q3A\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-Q3A/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_Q3A -DGAME_STR=\"Q3A\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-Q3A/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-Q3A/x86/,$(OBJ_FILES:.o=.d))
release64-Q3A: $(BIN_DIR)/release-Q3A/x86_64/$(BIN_64)_Q3A.so
debug64-Q3A: $(BIN_DIR)/debug-Q3A/x86_64/$(BIN_64)_Q3A.so

$(BIN_DIR)/release-Q3A/x86_64/$(BIN_64)_Q3A.so: $(addprefix $(OBJ_DIR)/release-Q3A/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-Q3A/x86_64/$(BIN_64)_Q3A.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-Q3A/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_Q3A -DGAME_STR=\"Q3A\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-Q3A/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_Q3A -DGAME_STR=\"Q3A\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-Q3A/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-Q3A/x86_64/,$(OBJ_FILES:.o=.d))
game-QUAKE2: release-QUAKE2 debug-QUAKE2
release-QUAKE2: release32-QUAKE2 release64-QUAKE2
debug-QUAKE2: debug32-QUAKE2 debug64-QUAKE2
release32-QUAKE2: $(BIN_DIR)/release-QUAKE2/x86/$(BIN_32)_QUAKE2.so
debug32-QUAKE2: $(BIN_DIR)/debug-QUAKE2/x86/$(BIN_32)_QUAKE2.so

$(BIN_DIR)/release-QUAKE2/x86/$(BIN_32)_QUAKE2.so: $(addprefix $(OBJ_DIR)/release-QUAKE2/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-QUAKE2/x86/$(BIN_32)_QUAKE2.so: $(addprefix $(OBJ_DIR)/debug-QUAKE2/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-QUAKE2/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_QUAKE2 -DGAME_STR=\"QUAKE2\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-QUAKE2/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_QUAKE2 -DGAME_STR=\"QUAKE2\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-QUAKE2/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-QUAKE2/x86/,$(OBJ_FILES:.o=.d))
release64-QUAKE2: $(BIN_DIR)/release-QUAKE2/x86_64/$(BIN_64)_QUAKE2.so
debug64-QUAKE2: $(BIN_DIR)/debug-QUAKE2/x86_64/$(BIN_64)_QUAKE2.so

$(BIN_DIR)/release-QUAKE2/x86_64/$(BIN_64)_QUAKE2.so: $(addprefix $(OBJ_DIR)/release-QUAKE2/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-QUAKE2/x86_64/$(BIN_64)_QUAKE2.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-QUAKE2/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_QUAKE2 -DGAME_STR=\"QUAKE2\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-QUAKE2/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_QUAKE2 -DGAME_STR=\"QUAKE2\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-QUAKE2/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-QUAKE2/x86_64/,$(OBJ_FILES:.o=.d))
game-RTCWMP: release-RTCWMP debug-RTCWMP
release-RTCWMP: release32-RTCWMP release64-RTCWMP
debug-RTCWMP: debug32-RTCWMP debug64-RTCWMP
release32-RTCWMP: $(BIN_DIR)/release-RTCWMP/x86/$(BIN_32)_RTCWMP.so
debug32-RTCWMP: $(BIN_DIR)/debug-RTCWMP/x86/$(BIN_32)_RTCWMP.so

$(BIN_DIR)/release-RTCWMP/x86/$(BIN_32)_RTCWMP.so: $(addprefix $(OBJ_DIR)/release-RTCWMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-RTCWMP/x86/$(BIN_32)_RTCWMP.so: $(addprefix $(OBJ_DIR)/debug-RTCWMP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-RTCWMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_RTCWMP -DGAME_STR=\"RTCWMP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-RTCWMP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_RTCWMP -DGAME_STR=\"RTCWMP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-RTCWMP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-RTCWMP/x86/,$(OBJ_FILES:.o=.d))
release64-RTCWMP: $(BIN_DIR)/release-RTCWMP/x86_64/$(BIN_64)_RTCWMP.so
debug64-RTCWMP: $(BIN_DIR)/debug-RTCWMP/x86_64/$(BIN_64)_RTCWMP.so

$(BIN_DIR)/release-RTCWMP/x86_64/$(BIN_64)_RTCWMP.so: $(addprefix $(OBJ_DIR)/release-RTCWMP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-RTCWMP/x86_64/$(BIN_64)_RTCWMP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-RTCWMP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_RTCWMP -DGAME_STR=\"RTCWMP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-RTCWMP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_RTCWMP -DGAME_STR=\"RTCWMP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-RTCWMP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-RTCWMP/x86_64/,$(OBJ_FILES:.o=.d))
game-RTCWSP: release-RTCWSP debug-RTCWSP
release-RTCWSP: release32-RTCWSP release64-RTCWSP
debug-RTCWSP: debug32-RTCWSP debug64-RTCWSP
release32-RTCWSP: $(BIN_DIR)/release-RTCWSP/x86/$(BIN_32)_RTCWSP.so
debug32-RTCWSP: $(BIN_DIR)/debug-RTCWSP/x86/$(BIN_32)_RTCWSP.so

$(BIN_DIR)/release-RTCWSP/x86/$(BIN_32)_RTCWSP.so: $(addprefix $(OBJ_DIR)/release-RTCWSP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-RTCWSP/x86/$(BIN_32)_RTCWSP.so: $(addprefix $(OBJ_DIR)/debug-RTCWSP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-RTCWSP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_RTCWSP -DGAME_STR=\"RTCWSP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-RTCWSP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_RTCWSP -DGAME_STR=\"RTCWSP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-RTCWSP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-RTCWSP/x86/,$(OBJ_FILES:.o=.d))
release64-RTCWSP: $(BIN_DIR)/release-RTCWSP/x86_64/$(BIN_64)_RTCWSP.so
debug64-RTCWSP: $(BIN_DIR)/debug-RTCWSP/x86_64/$(BIN_64)_RTCWSP.so

$(BIN_DIR)/release-RTCWSP/x86_64/$(BIN_64)_RTCWSP.so: $(addprefix $(OBJ_DIR)/release-RTCWSP/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-RTCWSP/x86_64/$(BIN_64)_RTCWSP.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-RTCWSP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_RTCWSP -DGAME_STR=\"RTCWSP\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-RTCWSP/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_RTCWSP -DGAME_STR=\"RTCWSP\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-RTCWSP/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-RTCWSP/x86_64/,$(OBJ_FILES:.o=.d))
game-SIN: release-SIN debug-SIN
release-SIN: release32-SIN
debug-SIN: debug32-SIN
release32-SIN: $(BIN_DIR)/release-SIN/x86/$(BIN_32)_SIN.so
debug32-SIN: $(BIN_DIR)/debug-SIN/x86/$(BIN_32)_SIN.so

$(BIN_DIR)/release-SIN/x86/$(BIN_32)_SIN.so: $(addprefix $(OBJ_DIR)/release-SIN/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-SIN/x86/$(BIN_32)_SIN.so: $(addprefix $(OBJ_DIR)/debug-SIN/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-SIN/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_SIN -DGAME_STR=\"SIN\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-SIN/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_SIN -DGAME_STR=\"SIN\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-SIN/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-SIN/x86/,$(OBJ_FILES:.o=.d))
game-SOF: release-SOF debug-SOF
release-SOF: release32-SOF
debug-SOF: debug32-SOF
release32-SOF: $(BIN_DIR)/release-SOF/x86/$(BIN_32)_SOF.so
debug32-SOF: $(BIN_DIR)/debug-SOF/x86/$(BIN_32)_SOF.so

$(BIN_DIR)/release-SOF/x86/$(BIN_32)_SOF.so: $(addprefix $(OBJ_DIR)/release-SOF/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-SOF/x86/$(BIN_32)_SOF.so: $(addprefix $(OBJ_DIR)/debug-SOF/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-SOF/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_SOF -DGAME_STR=\"SOF\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-SOF/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_SOF -DGAME_STR=\"SOF\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-SOF/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-SOF/x86/,$(OBJ_FILES:.o=.d))
game-SOF2MP: release-SOF2MP debug-SOF2MP
release-SOF2MP: release32-SOF2MP
debug-SOF2MP: debug32-SOF2MP
release32-SOF2MP: $(BIN_DIR)/release-SOF2MP/x86/$(BIN_32)_SOF2MP.so
debug32-SOF2MP: $(BIN_DIR)/debug-SOF2MP/x86/$(BIN_32)_SOF2MP.so

$(BIN_DIR)/release-SOF2MP/x86/$(BIN_32)_SOF2MP.so: $(addprefix $(OBJ_DIR)/release-SOF2MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-SOF2MP/x86/$(BIN_32)_SOF2MP.so: $(addprefix $(OBJ_DIR)/debug-SOF2MP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-SOF2MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_SOF2MP -DGAME_STR=\"SOF2MP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-SOF2MP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_SOF2MP -DGAME_STR=\"SOF2MP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-SOF2MP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-SOF2MP/x86/,$(OBJ_FILES:.o=.d))
game-SOF2SP: release-SOF2SP debug-SOF2SP
release-SOF2SP: release32-SOF2SP
debug-SOF2SP: debug32-SOF2SP
release32-SOF2SP: $(BIN_DIR)/release-SOF2SP/x86/$(BIN_32)_SOF2SP.so
debug32-SOF2SP: $(BIN_DIR)/debug-SOF2SP/x86/$(BIN_32)_SOF2SP.so

$(BIN_DIR)/release-SOF2SP/x86/$(BIN_32)_SOF2SP.so: $(addprefix $(OBJ_DIR)/release-SOF2SP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-SOF2SP/x86/$(BIN_32)_SOF2SP.so: $(addprefix $(OBJ_DIR)/debug-SOF2SP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-SOF2SP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_SOF2SP -DGAME_STR=\"SOF2SP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-SOF2SP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_SOF2SP -DGAME_STR=\"SOF2SP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-SOF2SP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-SOF2SP/x86/,$(OBJ_FILES:.o=.d))
game-STEF2: release-STEF2 debug-STEF2
release-STEF2: release32-STEF2
debug-STEF2: debug32-STEF2
release32-STEF2: $(BIN_DIR)/release-STEF2/x86/$(BIN_32)_STEF2.so
debug32-STEF2: $(BIN_DIR)/debug-STEF2/x86/$(BIN_32)_STEF2.so

$(BIN_DIR)/release-STEF2/x86/$(BIN_32)_STEF2.so: $(addprefix $(OBJ_DIR)/release-STEF2/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-STEF2/x86/$(BIN_32)_STEF2.so: $(addprefix $(OBJ_DIR)/debug-STEF2/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-STEF2/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_STEF2 -DGAME_STR=\"STEF2\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-STEF2/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_STEF2 -DGAME_STR=\"STEF2\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-STEF2/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-STEF2/x86/,$(OBJ_FILES:.o=.d))
game-STVOYHM: release-STVOYHM debug-STVOYHM
release-STVOYHM: release32-STVOYHM
debug-STVOYHM: debug32-STVOYHM
release32-STVOYHM: $(BIN_DIR)/release-STVOYHM/x86/$(BIN_32)_STVOYHM.so
debug32-STVOYHM: $(BIN_DIR)/debug-STVOYHM/x86/$(BIN_32)_STVOYHM.so

$(BIN_DIR)/release-STVOYHM/x86/$(BIN_32)_STVOYHM.so: $(addprefix $(OBJ_DIR)/release-STVOYHM/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-STVOYHM/x86/$(BIN_32)_STVOYHM.so: $(addprefix $(OBJ_DIR)/debug-STVOYHM/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-STVOYHM/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_STVOYHM -DGAME_STR=\"STVOYHM\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-STVOYHM/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_STVOYHM -DGAME_STR=\"STVOYHM\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-STVOYHM/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-STVOYHM/x86/,$(OBJ_FILES:.o=.d))
game-STVOYSP: release-STVOYSP debug-STVOYSP
release-STVOYSP: release32-STVOYSP
debug-STVOYSP: debug32-STVOYSP
release32-STVOYSP: $(BIN_DIR)/release-STVOYSP/x86/$(BIN_32)_STVOYSP.so
debug32-STVOYSP: $(BIN_DIR)/debug-STVOYSP/x86/$(BIN_32)_STVOYSP.so

$(BIN_DIR)/release-STVOYSP/x86/$(BIN_32)_STVOYSP.so: $(addprefix $(OBJ_DIR)/release-STVOYSP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-STVOYSP/x86/$(BIN_32)_STVOYSP.so: $(addprefix $(OBJ_DIR)/debug-STVOYSP/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-STVOYSP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_STVOYSP -DGAME_STR=\"STVOYSP\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-STVOYSP/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_STVOYSP -DGAME_STR=\"STVOYSP\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-STVOYSP/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-STVOYSP/x86/,$(OBJ_FILES:.o=.d))
game-WET: release-WET debug-WET
release-WET: release32-WET release64-WET
debug-WET: debug32-WET debug64-WET
release32-WET: $(BIN_DIR)/release-WET/x86/$(BIN_32)_WET.so
debug32-WET: $(BIN_DIR)/debug-WET/x86/$(BIN_32)_WET.so

$(BIN_DIR)/release-WET/x86/$(BIN_32)_WET.so: $(addprefix $(OBJ_DIR)/release-WET/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_32) -o $@ $(LDLIBS) $^
	
$(BIN_DIR)/debug-WET/x86/$(BIN_32)_WET.so: $(addprefix $(OBJ_DIR)/debug-WET/x86/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_32) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-WET/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_WET -DGAME_STR=\"WET\" $(REL_CFLAGS_32) -c $< -o $@

$(OBJ_DIR)/debug-WET/x86/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_WET -DGAME_STR=\"WET\" $(DBG_CFLAGS_32) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-WET/x86/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-WET/x86/,$(OBJ_FILES:.o=.d))
release64-WET: $(BIN_DIR)/release-WET/x86_64/$(BIN_64)_WET.so
debug64-WET: $(BIN_DIR)/debug-WET/x86_64/$(BIN_64)_WET.so

$(BIN_DIR)/release-WET/x86_64/$(BIN_64)_WET.so: $(addprefix $(OBJ_DIR)/release-WET/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(REL_LDFLAGS_64) -o $@ $(LDLIBS) $^	

$(BIN_DIR)/debug-WET/x86_64/$(BIN_64)_WET.so: $(addprefix $(OBJ_DIR)/debug-$(1)/x86_64/,$(OBJ_FILES))
	mkdir -p $(@D)
	$(CC) $(DBG_LDFLAGS_64) -o $@ $(LDLIBS) $^

$(OBJ_DIR)/release-WET/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(REL_CPPFLAGS) -DGAME_WET -DGAME_STR=\"WET\" $(REL_CFLAGS_64) -c $< -o $@

$(OBJ_DIR)/debug-WET/x86_64/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(DBG_CPPFLAGS) -DGAME_WET -DGAME_STR=\"WET\" $(DBG_CFLAGS_64) -c $< -o $@

-include $(addprefix $(OBJ_DIR)/release-WET/x86_64/,$(OBJ_FILES:.o=.d))
-include $(addprefix $(OBJ_DIR)/debug-WET/x86_64/,$(OBJ_FILES:.o=.d))
clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
