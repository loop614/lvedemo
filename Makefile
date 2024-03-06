CFLAGS = -std=c++17 # -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

LveDemo: main.cpp little_vulkan_engine/*.hpp little_vulkan_engine/*.cpp
	g++ $(CFLAGS) -o LveDemo main.cpp little_vulkan_engine/*.cpp $(LDFLAGS)

Shaders:  shaders/*.vert shaders/*.frag
	./shaders/compile.sh

demo: LveDemo Shaders
	./LveDemo

clean:
	rm -rf shaders/*.spv
	rm -f LveDemo
