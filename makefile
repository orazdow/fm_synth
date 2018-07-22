TARGET := Fm.exe

SRC := $(wildcard *.cpp)
SRCDIR_2 = D:/Libraries/PaWrapper
SRC += $(wildcard $(SRCDIR_2)/*.cpp)	

INCLUDE = -ID:/Libraries/portaudio/include 
INCLUDE += -I$(SRCDIR_2)
LIBS = -LD:/Libraries/portaudio/build 
LIBS += -lportaudio

CXX := g++
# FLAGS := -Wall

OBJECTS := $(addsuffix .o, $(basename $(notdir $(SRC))))
DEPS = $(wildcard *.h)


%.o: %.cpp $(DEPS)
	$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo $@

%.o: $(SRCDIR_2)/%.cpp $(DEPS)
	$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo $@

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

run:
	@./$(TARGET)