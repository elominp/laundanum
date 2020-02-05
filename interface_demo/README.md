# Example of breaking ABI in a C++ interface

## Introduction

The goal of these snippets of code and example is to show how some changes in C++ header files
can break an application even though visually looking we can think they're not harmful

## Part 1: Writing a two versions of an interface

For this example, we'll create a class named `Toto` and the interface `IToto` its implementing.
The definition of this class is in the `Toto-v1.hpp` header file.

Next, in the `Toto-v2.hpp` file we add a new method name `IShouldntBeThere` somewhere not at the the
end of the current definition but between two existing methods.

## Part 2: Let's comparing the size of the objects

In the two `sizeof_totov1.cpp` and `sizeof_totov2.cpp` we're printing how much bytes are used to instanciate our `Toto` and `IToto` classes (though the interface is just for demo as it's not instanciable).

Let's compile our first program using this command: `g++ sizeof_totov1.cpp -o sizeof_totov1`

Now, let's test it, it should print something like this:

```
$ ./sizeof_totov1
sizeof(IToto) = 8
sizeof(Toto) = 8
```

Let's compile our second program using this command: `g++ sizeof_totov2.cpp -o sizeof_totov2`

```
$ ./sizeof_totov2
sizeof(IToto) = 8
sizeof(Toto) = 8
```

Good! So adding a new method in our class doesn't change its size in memory! (Well, not where we instanciate our object as the size printed by sizeof is the size of the corresponding structure but adding a new method does change the memory layout of the vtable and member function pointers of the class as we'll se later)

## Part 3: Creating three programs demo programs

Now, to demonstrate the impact of adding a member function between existing definitions in a header file,
we will create two applications calling each of our declared methods for each version of the library.

Let's compile first version of the library using this command: `g++ -fPIC Toto-v1.cpp -c && g++ -shared -fPIC Toto-v1.o -o libtoto.so`

Now we compile the first program calling the functions of our `Toto` both of the object returned by the function instanciating it in the library and then the one instanciated in our program, implemented in the `call_totov1.cpp` file using this command: `g++ call_totov1.cpp -L. -ltoto -o call_totov1`

Running it should return something looking like this:

```
$ ./call_totov1
Call member functions of Toto object returned by getNewToto
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
func11: 0
Call member functions of Toto instanciated on the stack
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
func11: 0
```

Next we compile the new version of the library using this command: `g++ -fPIC Toto-v2.cpp -c && g++ -shared -fPIC Toto-v2.o -o libtoto.so`

We compile the second application, `call_totov2` using this command: `g++ call_totov2.cpp -L. -ltoto -o call_totov2`

And testing `call_totov2` should display something like this:

```
$ ./call_totov2
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
func11: 0
iShoudlntBeAddedThere: 0
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
func11: 0
iShoudlntBeAddedThere: 0
```

Good! The display on `call_totov2` works and print the call to our new function made at the end.

But what about `call_totov1`, sure it shouldn't have changed if we're calling it now, right?

Let's try running it once again *without* recompiling, it should print something like this:

```
$ ./call_totov1
Call member functions of Toto object returned by getNewToto
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
iShoudlntBeAddedThere: 6.64635e-33
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
Call member functions of Toto instanciated on the stack
func1: 0
func2: 0
func3: 0
func4: 0
func5: 0
iShoudlntBeAddedThere: 6.64635e-33
func6: 0
func7: 0
func8: 0
func9: 0
func10: 0
```

Now, we see that the output changed for both calls of the object instanciated in the libtoto and our application.

It seems like the function `iShouldntBeAddedThere` was called but it wasn't existing when our application was compiled, worse than this, with a wrong argument as the `0` we passed to all our functions calls in both applications is now a `6.64635e-33` value and it seems like our method `func10` wasn't called.

## Part 4: What happened?

After applications are compiled, two things occured at runtime:

- When classes are instanciated, a structure corresponding to its attributes is pushed on the memory where its allocated (either the stack or the heap if dynamically allocated), then its member functions are called by passing the structure to the corresponding function which in fact always contain a first hidden argument named `this` (yes, the `this` you're using to eventually access your class attributes and member functions in your implementation). That's why adding a new member function does not change the allocation size of the class as the corresponding functions are not located in the object itself
- When functions are called, they are in fact not directly called by the name the programmer gave them but by their address in memory, which for a class is an array of pointers of which each indexes corresponds to one of the member functions in the class definition, so in the second call of `call_totov1` something like this (the exact runtime behaviour is dependent of the compiler, it's not part of the C++ standard) happends:
  + `toto->func1(0);` -> `toto_functions[0](toto, 0(int))` -> OK, no changes (it's really `func1` in the library)
  + `toto->func2(0);` -> `toto_functions[1](toto, 0(int))` -> OK, no changes (it's really `func2` in the library)
  + `toto->func3(0);` -> `toto_functions[2](toto, 0(int))` -> OK, no changes (it's really `func3` in the library)
  + `toto->func4(0);` -> `toto_functions[3](toto, 0(int))` -> OK, no changes (it's really `func4` in the library)
  + `toto->func5(0);` -> `toto_functions[4](toto, 0(int))` -> OK, no changes (it's really `func5` in the library)
  + `toto->func6(0);` -> `toto_functions[5](toto, 0(int))` -> Ouch, in reality the address in the array points to `iShouldntBeAddedThere` and its expecting a float as the first argument and not an integer which are not represented the same way. The reason why the arguments are still passed besides the fact they are not the same type is because they are all the same size, especially for the first 6 arguments on a 64 bits processor as they are passed in CPU registers to every argument is 8 bytes long and something bigger is in fact pushed on the stack and the argument in the register is a pointer to this variable. The behaviour here is directly related to the CPU, so it will changes on different architectures such as ARM processors or even on older 32 bits x86 CPU where the calling convention was different.
  + `toto->func7(0);` -> `toto_functions[6](toto, 0(int))` -> The address points in reality to `func6` in the library
  + `toto->func8(0);` -> `toto_functions[7](toto, 0(int))` -> The address points in reality to `func7` in the library
  + And so on until we call `toto->func11` which is searched in `toto_functions[10]` in the array which in reality point to `func10` in the library and as the `func11` address is stored in `toto_functions[11]` which we never access we never call it.

What should we remember from this? Because in C++ classes functions are not called directly by their names (as in C by the way) but by addresses which here are already partially resolved at the compilation (at runtime the program only expects to resolve the location of the pointer arrays but already expects the corresponding addresses to be stored in the order they were during compilation), every single calls to member functions from where the `iShouldntBeAddedThere` function was put in place instead of the `func6` member function declaration were wrong as we were pointing on the wrong function and also passing incorrectly the arguments from the previously compiled applications.

That's why when member function definitions of a class need to be changed, the changes *must always* be made after the existing definitions either by overloading existing functions if it's parameter changes to an existing function or just adding the definition of a new member function