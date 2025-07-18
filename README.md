# gr-hsdec
### High-Speed data decoder



## Dependents

    gnuradio 3.10
    cmake, make, git etc...

## Build && Install
run in Terminal:

    git clone https://github.com/CLA-179/gr-hsdec.git
    cd gr-hsdec
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    sudo ldconfig