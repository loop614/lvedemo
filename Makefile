CFLAGS := -std=c++17 #-O3 -I$(shell pwd)
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

LveDemo: main.cpp little_vulkan_engine/*.hpp little_vulkan_engine/*.cpp
	g++ $(CFLAGS) -o LveDemo main.cpp little_vulkan_engine/*.cpp $(LDFLAGS)

LveShaders:  shaders/*.vert shaders/*.frag
	/usr/bin/glslc shaders/shader.vert -o shaders/vert.spv
	/usr/bin/glslc shaders/shader.frag -o shaders/frag.spv

PointShaders:  shaders_point/*.vert shaders_point/*.frag
	/usr/bin/glslc shaders_point/shader.vert -o shaders_point/vert.spv
	/usr/bin/glslc shaders_point/shader.frag -o shaders_point/frag.spv

demo: PointShaders LveShaders LveDemo
	./LveDemo

clean:
	rm -rf shaders/*.spv
	rm -f LveDemo
