# cpp-mython
Mython interpreter. Creating my own DSL.

Read in other languages: [English](README.md), [Русский](README.Russian.md)

# Program Description
Mython(Mini-python. There is a transcript of My Python, but it's about another Mython) — a simplified subset of Python.<br>
In Mython there are **classes** and **inheritance**, and all **methods** are **virtual**.<br>

# Assembly using Cmake
To build this project on linux you need:<br>
1)If you don't have Cmake installed, install Cmake<br>
2)If the "Debug" or "Release" folders are not created:<br>

```
mkdir Debug
mkdir Release
```
3)Run the command for Debug and Release conf:<br>

```
cmake -E chdir Debug/ cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE:STRING=Debug
cmake -E chdir Release/ cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE:STRING=Release
```
4)Build command:<br>

```
cmake --build Debug/.
cmake --build Release/.
```

5)To **Run** program- go to the debug (cd Debug/) or release (cd Release/) folder and run:<br>

```
./mython
```
6)After the messages about the successful completion of the tests, you can run your program in Python, and then, at the end of the program, to get an answer, type **ctrl+D into the command console**<br>
<br>

**ALL in one command(Release)**:<br>

```
mkdir Release; cmake -E chdir Release/ cmake -G "Unix Makefiles" ../ -DCMAKE_BUILD_TYPE:STRING=Release && cmake --build Release/.
```

# System requirements:
  1. C++17(STL)<br>
  2. GCC (MinG w64) 11.2.0  <br>
  
# Plans for completion:
1. Add UI<br>
2. Add the ability to read documents from files<br>
3. Add support for fractional numbers<br>
4. Add more arithmetic operations<br>

# Technology stack:
1. OOP<br>
2. Abstract Syntax Tree (AST).<br>
3. Inheritance<br>
4. Table of virtual methods<br>
5. Lexical analyzer, or lexer.<br>
6. Parser, or parser.<br>
7. Semantic analyzer.<br>