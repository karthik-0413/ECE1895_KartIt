ifeq ($(OS),Windows_NT)
    EXE := ../super_mario_kart.exe
    RM := del /s
    MKDIR := mkdir
else
    EXE := ../super_mario_kart
    MKDIR := mkdir -p
endif

BINDIR := ../bin

# Collect all .cpp files recursively and in the current directory,
# and explicitly add map/checkpoint.cpp and map/checkpointpausestate.cpp
SRC := $(wildcard **/*.cpp) $(wildcard *.cpp) map/checkpoint.cpp map/checkpointpausestate.cpp
OBJ := $(patsubst %.cpp, $(BINDIR)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)

# SFML settings
SFML_INCLUDE := -IC:/SFMLstatic/include
SFML_LIB := -LC:/SFMLstatic/lib

# Append SFML include to CXXFLAGS and library path to LDFLAGS.
CPPFLAGS :=
CPPFLAGS += $(SFML_INCLUDE) 
LDFLAGS += $(SFML_LIB)

INCFLAGS := -IC:/mingw64/include -Iinclude
LIBFLAGS := -LC:/mingw64/bin

DEFINES := -DSFML_STATIC

# Link against SFML modules (static version in release on Windows).
# In debug mode we use the dynamic versions (with no -s suffix).
#LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 

.PHONY: all clean debug release

all: debug

debug:
	$(MAKE) folders
	$(MAKE) $(EXE)

release: CPPFLAGS += -DSFML_STATIC
release: CXXFLAGS += -O3 -pedantic
release: LDLIBS :=
ifeq ($(OS),Windows_NT)
    # For Windows release, force static linking for SFML:
release: LDFLAGS := -mwindows -static
release: LDLIBS := -static-libgcc -static-libstdc++ -pthread
release: LDLIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32
release: LDLIBS += -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32
release: LDLIBS += -lsfml-audio-s -lsfml-system-s -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg
release: LDLIBS += -lsfml-system-s -lwinmm
else
    # For Unix-like systems; statically link libgcc/libstdc++
    release: LDLIBS += -static-libgcc -static-libstdc++ -pthread
    release: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
endif
release: folders $(EXE)

folders: $(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio
$(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio:
	$(MKDIR) "$@"

ifeq ($(OS),Windows_NT)
$(EXE): $(OBJ) $(BINDIR)/super_mario_kart.res
	$(CXX) $(LDFLAGS) $^ $(BINDIR)/super_mario_kart.res $(LDLIBS) -o $@
else
$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
endif

# Generic rule for building .o files from .cpp files.
$(BINDIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

# Explicit rule for map/checkpoint.cpp to ensure $(BINDIR)/map/checkpoint.o is built.
$(BINDIR)/map/checkpoint.o: map/checkpoint.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

# Explicit rule for map/checkpointpausestate.cpp to ensure $(BINDIR)/map/checkpointpausestate.o is built.
$(BINDIR)/map/checkpointpausestate.o: map/checkpointpausestate.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

$(BINDIR)/super_mario_kart.res: super_mario_kart.rc
	windres super_mario_kart.rc -O coff -o $(BINDIR)/super_mario_kart.res

ifeq ($(OS),Windows_NT)
clean:
	if exist "..\bin" rmdir /S /Q "..\bin"
	if exist "..\super_mario_kart.exe" del "..\super_mario_kart.exe"
else
clean:
	$(RM) $(EXE) $(OBJ) $(DEP)
endif
