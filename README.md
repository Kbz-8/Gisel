# NIR

Nir is a small scripting language inspired by the C programming language.
Unlike C, it has no pointers but references instead. It was made for fun for school and is not really serious.
Its interpreter is pretty fast because it's written in C++.
There is an API to interact between C++ and Nir that allows you to write C++ functions and run them in your Nir code.

Here's an example of Nir code

``` Rust
fn swap(var& x : num, var& y : num) -> void // params given by reference
{
    var temp : num = x;
    x = y;
    y = temp;
}

exteral fn main() -> void
{
    var i : num = 42;
    var j : num = 12;
    swap(:i, :j); // pass references to i and j
    print(to_str(i));
    print(to_str(j));
}
```
