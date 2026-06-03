CXX     = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS  = -framework OpenGL -framework GLUT
TARGET   = triangle
SRCS     = main.cpp piplup.cpp

$(TARGET): $(SRCS) piplup.h
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
