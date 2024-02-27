CFLAGS = -std=c++17 # -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

LveDemo: *.cpp *.hpp
	g++ $(CFLAGS) -o LveDemo *.cpp $(LDFLAGS)

Shaders:  shaders/shader.vert shaders/shader.frag
	./shaders/compile.sh

demo: LveDemo Shaders
	./LveDemo

clean:
	rm -rf shaders/*.spv
	rm -f LveDemo
