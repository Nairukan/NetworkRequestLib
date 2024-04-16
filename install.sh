sudo rm -rf build/*
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release
sudo make -C build/
sudo make -C build/ install
sudo rm -rf build/*
cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Debug
sudo make -C build/
sudo make -C build/ install
