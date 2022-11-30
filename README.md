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
6)After the messages about the successful completion of the tests, you can run your program in Mython, and then, at the end of the program, to get an answer, type **ctrl+D into the command console**<br>
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

# Usage
## Before you start:
  0. Installation and configuration of all required components in the development environment to run the application<br>
  1. The use case is shown in main.cpp, lexer_test_open.cpp, parse_test.cpp, parse_test.cpp, parse_test.cpp<br>
  
# Description of features:
## Numbers
Python uses only integers. You can perform the usual arithmetic operations with them: addition, subtraction, multiplication, integer division.<br>

## Lines in Mython are unchangeable.
A string constant in Mython is a sequence of arbitrary characters placed on a single line and bounded by double quotes " or single '. Escaping of special characters '\n', '\t', '\\" and '\\"' is supported. [example 1](#1-examples-of-lines-in-mython) <br>

## Logical constants and None
In addition to string and integer values, the Mython language supports boolean values **True** and **False**. There is also a special value **None**, analogous to nullptr in C++. Unlike C++, logical constants are written with a capital letter.<br>

## Comments
Mython supports single-line comments starting with the # character. All the following text up to the end of the current line is ignored. # inside strings is considered a regular character. [example 2](#2-examples-of-comments-in-mython)<br>

## Identifiers
Identifiers in Mython are used to denote the names of variables, classes, and methods. Identifiers are formed in the same way as in most other programming languages: they begin with a lowercase or uppercase Latin letter or with an underscore. Then follows an arbitrary sequence consisting of numbers, letters and an underscore. [example 3](#3-examples-of-identifiers-in-mython)<br>

## Classes
In Mython, you can define your type by creating a class. As in C++, the class has fields and methods, but, unlike in C++, fields do not need to be declared in advance.<br>
The class declaration begins with the keyword class, followed by the name identifier and the declaration of the methods of the class.<br>
<br>
Important differences between classes in Mython and classes in C++:
- The special method **__init__** plays the role of a constructor — it is automatically called when creating a new class object. The **__init__** method may be missing.
- An implicit parameter of all methods is a special parameter **self**, analogous to the this pointer in C++. The **self** parameter refers to the current object of the class.<br>
- Fields are not declared in advance, but are added to the class object at the first assignment. Therefore, accesses to class fields should always start with **self**. to distinguish them from local variables.<br>
- All fields of the object are public.<br>
A new object of a previously declared class is created in the same way as in C++: by specifying the name of the class, followed in parentheses by the parameters passed to the **__init__** method.<br>

```
r = Rect(10, 5)
```
This program creates a new object of the Rect class. When calling the **__init__** method, the w parameter will have the value 10, and the h parameter will have the value 5. The created rectangle will be available in the r variable. [example 4](#4-example-of-the-rectangle-class)<br>

## Typing
Unlike C++, Mython is a dynamic typing language. In it, the type of each variable is determined during the execution of the program and can change during its operation. Therefore, instead of "assigning a value to a variable", it is better to talk about "associating a value with some name". Thanks to dynamic typing, when using a variable for the first time, it is not necessary to specify its type. [example 5](#5-example-of-dynamic-typing)<br>

## Operations
In Mython defined:<br>
- Arithmetic operations for integers, division is performed entirely. Dividing by zero causes a runtime error.<br>
- String concatenation operation, for example: `s = 'hello, ' + 'world'.`<br>
- String and integer comparison operations `==, !=, <=, >=, <, >`; string comparison is performed - lexicographically.<br>
- Logical operations `and, or, not'.<br>
- Unary minus.<br>

Priority of operations (in descending order of priority):<br>
- Unary minus.<br>
- Multiplication and division.<br>
- Addition and subtraction.<br>
- Comparison operations.<br>
- Logical operations.<br>

The order of expression evaluation can be changed by parentheses:<br>
`print 2 + 3 * 4 # will output 14`<br>
`print (2 + 3) * 4 # will output 20 `<br>

In Mython, the addition operation, except for numbers and strings, is applicable to class objects with a special method **__add__** [example 6](#6-example-of-an-operation-for-adding-class-objects-with-a-special-method-add)<br>

Comparison operations are applied not only to numbers and strings, but also to objects of classes having methods **__eq__** (check "equal") and **__lt__** (check "less"). Using these methods, you can implement all comparison operations. [example 7](#7-example-of-applying-a-comparison-operation-to-class-objects) <br>

## str function
The **str** function converts the argument passed to it into a string. If the argument is a class object, it calls a special method **__str__** from it and returns the result. If there is no **__str__** method in the class, the function returns a string representation of the address of the object in memory. Examples:<br>
- `str('Hello') returns the string Hello;`<br>
- `str(100500) returns string 100500;`<br>
- `str(False) returns the string False;`<br>
- `str(Rect(3, 4)) will return the address of the object in memory, for example 0x2056fd0.`<br>
[example 8](#8-example-of-a-class-with-the-str-method)<br>

## Print command
The special command **print** accepts a set of comma-separated arguments, prints them to standard output and additionally outputs a line feed. Take a look at this code:<br>

```
x = 4
w = 'world'
print x, x + 6, 'Hello, ' + w
```
It will output:<br>
`4 10 Hello, world `<br>
The **print** command inserts a space between the output values. If you don't pass arguments to it, it will just output a line feed.<br>
To convert each of its arguments into a string, the **print** command calls the **str** function for it. Thus, the command `print Rect(20, 15)` will output the string `Rect(20x15)` to stdout.<br>

## Conditional operator
There is a conditional operator in Mython. [example 9](#9-example-of-using-a-conditional-operator)<br>

## Inheritance
In the Mython language, a class can have one parent class. If there is one, it is indicated in parentheses after the class name and before the colon character.<br>
Inheritance in Mython works the same way as in C++ — all methods of the parent class become available to the descendant class. At the same time, all methods are public and virtual. [example 10](#10-in-the-example-below-the-rect-class-inherits-from-the-shape-class)<br>

## Methods
Methods in Mython have syntax:

```
def <method name>(<parameter list>):
  <action 1>
  <action 2>
...
<action N>
```
The keyword **def** is indented with two spaces relative to the class. The instructions that make up the method body are indented with two spaces relative to the keyword **def**.<br>
As in the case of class fields, accessing the fields and methods of the current class should start with self.:<br>

```
class Factorial:
  def calc(n):
    if n == 0:
      return 1
    return n * self.calc(n - 1)

fact = Factorial()
print fact.calc(4) # Prints 24 
```
This example also shows support for recursion, which compensates for the lack of loops in the language.<br>
The return command terminates the execution of the method and returns the result of calculating its argument from it. If the execution of the method does not reach the return command, the method returns **None**.<br>

## Assignment semantics
As mentioned above, Mython is a language with dynamic typing, so the assignment operation has the semantics not of copying a value to a memory area, but of associating a variable name with a value. As a consequence, variables only refer to values, and do not contain copies of them. In C++ terminology, variables in Mython are pointers. The analog of nullptr is the value **None**. [example 11](#11-example-of-assignment-semantics)<br>

## Other restrictions
The result of calling a method or constructor in Mython is a terminal operation. Its result can be assigned to a variable or used as a function parameter or command, but it is not possible to directly access the fields and methods of the returned object:<br>

```
# It's not right
print Rect(10, 5).w
# And this is how you can
r = Rect(10, 5)
print r.w
```

# Structure
## The interpreter consists of four main logic blocks:
- Lexical analyzer, or lexer.<br>
- Parser, or parser.<br>
- Semantic analyzer.<br>
- Symbol table.<br>

### Lexical analysis
The first phase is called lexical analysis or scanning. Lexical analyzer — reads the sequence of characters that make up the source program, and forms meaningful sequences of characters — lexemes from them. For each token, the analyzer builds an output token of the form:<br>
`<token name, attribute value>` <br>
This token is passed to the next phase, parsing. The token name is an abstract symbol used during parsing, and the optional attribute value contains additional information related to the token.<br>

### Syntactic analysis
The second phase is called syntactic analysis or parsing. The analyzer — also called a parser — uses tokens obtained during lexical analysis to create an intermediate representation. It describes the grammatical structure of the token stream. Typically, such a representation is an abstract syntactic tree, a tree—like data structure in which each internal node specifies an operation, and child nodes are the arguments of this operation.<br>

### Semantic analysis
The semantic analyzer interprets the program. It sequentially traverses the syntax tree, performing actions related to the nodes of the tree, and updates the **symbol table**.<br>

# Examples
### 1. Examples of lines in Mython:
```
"hello"
'world'
'long string with a double quote " inside'
"another long string with a single quote ' inside"
"string with a double quote \" inside"
'string with a single quote \' inside'
'', "" — empty strings.
```

### 2. Examples of comments in Mython:
```
# this is a comment
x = 5 #this is also a comment
# in the next line # is a regular character
hashtag = "#nature"
 ```
### 3. Examples of identifiers in Mython:
```
Examples of correct identifiers: 
x, _42, do_something, int2str. 
Examples of incorrect identifiers:
4four — starts with a digit;
one;two — contains a character that does not refer to numbers, letters, or underscores.
```
### 4. Example of the "Rectangle" class:
```
class Rect:
  def __init__(w, h):
    self.w = w
    self.h = h

  def area():
    return self.w * self.h   
```
### 5. Example of dynamic typing:
```
x = 4 # variable x is associated with integer value 4
# the following command binds the variable x to the value 'hello'
x = 'hello'
y = True
x = y 
```
### 6. Example of an operation for adding class objects with a special method **__add__**:
```
   class Fire:
  def __init__(obj):
    self.obj = obj

  def __str__():
    return "Burnt " + str(self.obj)

class Tree:
  def __str__():
    return "tree"

class Matches: 
  # the operation of adding matches with other objects turns them into fire
  def __add__(smth):
    return Fire(smth)

result = Matches() + Tree()
print result             # Outputs Burnt tree
print Matches() + result # Outputs Burnt Burnt tree
```
### 7. Example of applying a comparison operation to class objects: 
```
class Person:
  def __init__(name, age):
    self.name = name
    self.age = age
  def __eq__(rhs):
    return self.name == rhs.name and self.age == rhs.age
  def __lt__(rhs):
    if self.name < rhs.name:
        return True
    return self.name == rhs.name and self.age < rhs.age

print Person("Ivan", 10) <= Person("Sergey", 10) # True
print Person("Ivan", 10) <= Person("Sergey", 9)  # False
```
### 8. Example of a class with the __str__ method:
```
class Rect(Shape):
  def __init__(w, h):
    self.w = w
    self.h = h

  def __str__():
    return "Rect(" + str(self.w) + 'x' + str(self.h) + ')' 
```
The expression str(Rect(3, 4)) returns the string Rect(3x4).<br>

### 9. Example of using a conditional operator:
Syntax:<br>

```
if <condition>:
  <action 1>
  <action 2>
  ...
  <action N>
else:
  <action 1>
  <action 2>
  ...
  <action M>
```

**<condition>** is an arbitrary expression followed by a colon. If the condition is true, actions are performed under the **if** branch, if false, actions are performed under the **else** branch. The presence of the **else** branch is optional.<br>
**<condition>** can contain comparisons, as well as logical operations **and**, **or** and **not**. The condition will be true or false depending on what type the calculated expression has.
If the result of calculating the condition is a boolean value, it is used to verify the truth of the condition. Examples:<br>
- `if x > 0:`<br>
- `if s != 'Jack' and s != 'Ann':`<br>
If the result of calculating the condition is a number, the condition is true if and only if this number is not zero, as in C/C++, for example, `if x + y:`.<br>
If the result of calculating the condition is a string, the condition is true if and only if this string has a non—zero length.<br>
If the result of calculating the condition is an object of the class, the condition is true.<br>
If the result of the condition calculation is None, the condition is false.<br>
The actions in the **if** and **else** branches are typed with two spaces indented. Unlike C++, in which code blocks are framed by curly brackets, in the Mython language commands are combined into indented blocks. One indent is equal to two spaces. An indentation of an odd number of spaces is considered incorrect.<br> Compare:

```
if x > 0:
  x = x + 1
print x

if x > 0:
  x = x + 1
  print x 
```
The first command `print x` will always be executed, the second — only if x is greater than 0. The nesting of conditions can be arbitrary:<br>

```
if x > 0:
  if y > 0:
    print "This line will be output if x and y are positive"
else:
  print "This line will be output if x <= 0"
```

### 10. In the example below, the Rect class inherits from the Shape class:
```
class Shape:
  def __str__():
    return "Shape"

  def area():
    return 'Not implemented'

class Rect(Shape):
  def __init__(w, h):
    self.w = w
    self.h = h

  def __str__():
    return "Rect(" + str(self.w) + 'x' + str(self.h) + ')'

  def area():
    return self.w * self.h 
```

All methods are public and virtual. For example, the code below will output Hello, John:<br>

```
class Greeting:
  def greet():
    return "Hello, " + self.name()

  def name():
    return 'Noname'

class HelloJohn(Greeting):
  def name():
    return 'John'

greet_john = HelloJohn()
print greet_john.greet()
```
### 11. Example of assignment semantics:
The code below will output 2, since the variables x and y refer to the same object:

```
class Counter:
  def __init__():
    self.value = 0

  def add():
    self.value = self.value + 1

x = Counter()
y = x
x.add()
y.add()
print x.value 
```
  
