redeploy-release-local:
	rm -rf build/
	mkdir build/ && \
	cd build/ && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 -DCMAKE_C_COMPILER=clang-10 .. && \
	make && sudo make install

deploy-release-local:
	cd build/ && make && sudo make install

build-and-install:
	mkdir build/ && \
	cd build/ && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 -DCMAKE_C_COMPILER=clang-10 .. && \
	make && sudo make install

run-example:
	rm -rf example/build/ && mkdir example/build/ && cd example/build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 -DCMAKE_C_COMPILER=clang-10 .. && \
	make && ./xalwart-example-server

run-example-valgrind:
	rm -rf example/build/ && mkdir example/build/ && cd example/build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-10 -DCMAKE_C_COMPILER=clang-10 .. && \
	make && valgrind --leak-check=full ./xalwart-example-server
