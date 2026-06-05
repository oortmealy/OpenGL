CXX     = g++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS  = -framework OpenGL -framework GLUT
TARGET   = piplup
SRCS     = main.cpp piplup.cpp body.cpp face.cpp
HDRS     = piplup.h body.h face.h

$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
