# ReTools

## Introduction
This is a set of tools intended for reverse engineering using DLL injection on Windows.
## Building and Installation

Navigate into the project's root directory  

Steps for 32 bit:
* Configure
```
cmake -S . -B out/build/x86 -A Win32
```
* Build
```
cmake --build out/build/x86 --config=Release
```
* Install
```
cmake --install out/build/x86 --prefix="path/to/install"
```
Steps for 64 bit:
* Configure
```
cmake -S . -B out/build/x64 -A x64
```
* Build
```
cmake --build out/build/x64 --config=Release
```
* Install
```
cmake --install out/build/x64 --prefix="path/to/install"
```

# Examples
## Hooking with a function pointer
```c++
int addTwo(int a, int b){
  return a + b;
}

utility::HookFunction<int> addTwoEdited((addr_t)addTwo);

addTwoEdited.setHook([](int a, int b){
  //call the original function
  auto ret = addTwoEdited(a, b);
  
  //log parameters
  std::cout << "first param: " << a << ", second param: " << b << ", return value: " << ret << '\n';
  
  //modify return value
  return ret * 2;
});
```
## Hooking with an address and a calling convention (x86)
```c++
//address of an unknown function returning int, accepting a vector of double and using stdcall calling convention
addr_t fncAddr = 0xdeadbeef;

utility::HookFunction<int> someFnc(fncAddr);

someFnc.setHook(stdcall_([](std::vector<double>& param) {
  //modify first value
  if(!param.empty()) param[0] *= 10;
  
  //call the original function
  auto ret = someFnc(param);
  
  //log parameters
  std::cout <<"Parameters:\n";
  
  for(auto p : param){
    std::cout << p << ' ';
  }
  std::cout << "Returns " << ret << '\n';
  return ret;
}));
```
## Dynamic stack trace
```c++
someFnc.setHook([]() {
  addr_t stackPointer;
  GET_STACK_POINTER(stackPointer);
  //trace 5 calls back
  auto stack = getAddrOfLastNCalls(stackPointer, cmm, 5);
  
  for(auto address : stack){
    std::cout << address << '\n'; 
  }
});
```
