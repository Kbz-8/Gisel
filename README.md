# Genuine Interpreted Shitty Educational Language

Gisel is a small scripting language inspired by the C programming language.
Unlike C, it has no pointers but references instead. It was made for fun for school and is not really serious.
Its interpreter is pretty fast because it's written in C++.
There is an API to interact between C++ and Gisel that allows you to write C++ functions and run them in your Gisel code.

Here's an example of Gisel code

``` Rust
import "std_memory.gisel"; // imports std_swap

export fn main() -> void
{
    var i : num = 42;
    var j : num = 12;
    std_swap(:i, :j); // pass references to i and j
    print(to_str(i));
    print(to_str(j));
}
```
