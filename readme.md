## hust-cs-network lab2: FromStopWait
### environment:
- [optinal] `ubuntu 20.04` (However, I use it through WSL)
- `cmake` >= `3.10`
- `gcc` & `g++` (`8.4.0` is ok in my environment)

### usage
First, clone the repo:
```bash
git clone https://github.com/N-E-E/FromStopWait.git
cd FromStopWait
```
Then build:
```bash
mkdir build && cd build
cmake ..
make
```
Run:
```bash
cd bin
./stop_wait or ./gbn or ./sr or ./tcp
```

### notes
- if you build under windows env, choose `sljl` mingw, or you may meet `undefined refrence` problem from the static lib file.
