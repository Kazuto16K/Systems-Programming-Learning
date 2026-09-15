# Extras — Theory Notes (companion to extras.cpp)

Concepts covered in `extras.cpp`, explained in more depth than the inline
comments. This is reference material, not new exercises.

---

## 1. `std::array<T, N>` — fixed-size arrays

```cpp
std::array<int, 5> data;
```

A `std::array` wraps a plain C-style array (`int data[5]`) in a class with
STL-container conveniences (`.size()`, iterators, bounds-checked `.at()`),
but the size `N` is fixed **at compile time** as a template parameter — it
lives entirely on the stack, with no heap allocation at all.

**When to use it over `std::vector`:**
- You know the exact size up front and it never changes.
- You want to guarantee no heap allocation (matters in performance-critical
  or embedded-style code).
- You want value semantics that are cheap and predictable — copying a
  `std::array` is just copying stack bytes, no pointer/heap involved.

This is the fixed-capacity counterpart to the `DynamicArray<T>` you built in
`task.cpp` — same idea (indexed, contiguous, typed storage), but no
growth logic because none is needed.

---

## 2. Function pointers

```cpp
void HelloWorld() { ... }
void (*function)() = HelloWorld;
function(); // calls HelloWorld
```

A function pointer stores the **address of a function**, so you can pass
functions around as values — store them in variables, pass them as
arguments, put them in containers. The declaration syntax reads
inside-out: `void (*function)()` — "`function` is a pointer to a function
taking no arguments and returning `void`."

**Why this matters:** it's what makes the *callback* pattern possible —
a function that takes another function as a parameter and calls it later:

```cpp
void ForEach(const std::vector<int>& values, void(*func)(int)) {
    for (int value : values)
        func(value);
}
```

`ForEach` doesn't know or care *what* `func` does — it just knows the
*shape* (takes an `int`, returns `void`) and calls it once per element.
This is the same underlying idea (indirect calls) that C libraries — like
LMDB or ONNX Runtime's C API — use extensively for callbacks, comparator
functions, and custom allocators. Understanding function pointers here is
a direct on-ramp to reading and wrapping C APIs later.

---

## 3. Lambdas

```cpp
auto lambda = [](int value) {
    std::cout << "value: " << value << std::endl;
};
```

A lambda is an anonymous, inline function — useful when you need a small
one-off function (like a callback for `ForEach`) without formally
declaring a named function elsewhere. Under the hood, the compiler
generates a small unnamed class with an `operator()` — a lambda **is**
an object of that generated class, which is also why a *capture-less*
lambda can implicitly convert to a plain function pointer (as used in your
`ForEach(values, [](int value){ ... });` call).

### Capture list — the `[...]` at the start

The capture list controls what *outside* variables the lambda body is
allowed to see, and how:

| Capture | Meaning |
|---|---|
| `[]` | Captures nothing — body can only use its own parameters/locals. |
| `[a]` | Captures `a` **by value** — a copy, frozen at the point the lambda is created. |
| `[&a]` | Captures `a` **by reference** — sees live updates, but dangerous if the lambda outlives `a`. |
| `[=]` | Captures **everything** used from the enclosing scope, by value. |
| `[&]` | Captures **everything** used from the enclosing scope, by reference. |
| `[a, &b]` | Mix: `a` by value, `b` by reference. |

**Value vs. reference capture matters a lot in practice:** a `[&]`
lambda that outlives the scope it was created in (e.g. stored and called
later, or run on another thread) is a dangling-reference bug waiting to
happen — the same class of bug as Day 1's dangling pointer, just hidden
inside a closure instead of a raw pointer. Default to `[=]`/explicit
by-value captures unless you specifically need to observe or mutate the
original variable.

---

## 4. Namespaces

```cpp
namespace apple {
    void print(const std::string& text) { ... }
}
namespace orange {
    void print(const std::string& text) { ... }
}
```

Two functions with the identical signature `print(const std::string&)`
would collide at global scope — the compiler wouldn't know which one you
mean. Namespaces give each a distinct qualified name (`apple::print`,
`orange::print`), so both can coexist. This becomes essential once you're
pulling in third-party libraries (exactly the situation in Semantic CLI,
which vendors LMDB, ONNX Runtime, and PDFium) — namespaces are the
mechanism that keeps one library's `Storage` class from colliding with
another's.

An **anonymous namespace** (`namespace { ... }`, no name) is a common
pattern in `.cpp` files for giving something *internal linkage* — visible
only within that translation unit, never clashing with anything in another
file. It's the modern C++ replacement for `static` at file scope.

---

## 5. Macros (`#define`)

```cpp
#define WAIT std::cin.get()
```

The preprocessor performs a pure **textual substitution** before
compilation even begins — every occurrence of `WAIT` in the source is
literally replaced with `std::cin.get()` before the compiler sees it.
Macros have no type-checking, no scoping rules, and no awareness of C++
syntax — which is exactly why modern C++ prefers `const`/`constexpr` for
constants and real functions (or templates) for logic, reserving macros
for the handful of things only the preprocessor can do (conditional
compilation with `#ifdef`, include guards, and stringizing/token-pasting
tricks).

---

## 6. `std::reverse` (from `<algorithm>`)

```cpp
std::reverse(temp.begin(), temp.end());
```

Part of the STL's algorithm library — operates on a pair of iterators
(`begin`, `end`) rather than a specific container type, which is why the
exact same call works whether `temp` is a `std::string`, `std::vector`,
or any other type exposing iterators. This iterator-based design is the
core idea behind the whole `<algorithm>` header: algorithms and containers
are decoupled, so one `reverse`/`sort`/`find` implementation works across
every container type.
