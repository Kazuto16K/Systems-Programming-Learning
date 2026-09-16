# Day 5 — Binary File I/O: Theory Notes

Concepts learned while building `task05.cpp`.

---

## 1. Binary vs. text I/O

Text I/O (`std::cout << 42`) converts values into human-readable characters
(`'4'`, `'2'`) before writing them. **Binary I/O** writes the raw in-memory
bytes of a value directly — a 4-byte `int` becomes exactly 4 bytes on disk,
no conversion, no parsing.

```cpp
std::ofstream out("records.txt");                     // text mode
std::ofstream out("records.bin", std::ios::binary);   // binary mode
```

`std::ios::binary` matters on Windows specifically: without it, the stream
may silently translate `\n` bytes into `\r\n`, corrupting any binary data
that happens to contain that byte pattern.

**Why use it:** compact (fixed size per value, no digit-string overhead),
fast (no parsing), and — the key reason for today — it lets you write/read
a whole array of structs in a single call, because the bytes on disk are
laid out exactly like the bytes in memory.

---

## 2. POD structs — why `char[32]` instead of `std::string`

```cpp
struct Record {
    uint32_t id;
    float score;
    char name[32];
};
```

A **POD** (Plain Old Data) type has no pointers, no virtual functions, no
user-defined constructors — its bytes fully describe its value, so it's
safe to copy as raw bytes.

`char name[32]` stores the actual characters *inline*, as part of the
struct. `std::string` would instead store a *pointer* to a heap buffer —
writing that pointer's raw bytes to disk would just write a memory address
that means nothing once the program exits. This is the same "resource
behind a pointer doesn't survive naive byte-copying" lesson from Day 3's
`String` class and Day 4's `DynamicArray`.

Fixed-size fields also mean every `Record` is exactly the same number of
bytes — so record *N* always starts at byte `N * sizeof(Record)` in the
file. That's what makes a binary file trivially seekable/indexable later.

---

## 3. Writing raw bytes: `ofstream::write`

```cpp
out.write(reinterpret_cast<const char*>(record), count * sizeof(Record));
```

Signature: `ostream& write(const char* data, std::streamsize count)` —
"starting at address `data`, copy `count` bytes into the file."

`write()` requires a `const char*`, but `record` is a `const Record*` — so
`reinterpret_cast` tells the compiler "treat this memory as a raw byte
sequence, ignore its actual type." This is deliberately the most
permission-bypassing cast in C++, because that's genuinely what binary I/O
needs: the type system doesn't understand "bytes on disk," only C++ objects.

## 4. Reading raw bytes back: `ifstream::read`

```cpp
in.read(reinterpret_cast<char*>(records.data()), count * sizeof(Record));
```

Mirror image of `write()`: "read `count` bytes from the file into the
memory starting at `records.data()`." Since the byte layout written
matches a `Record`'s in-memory layout exactly, the struct's fields come
back populated correctly with no parsing step.

**Verifying a full read actually happened** — `gcount()` returns how many
bytes the last `read()` call actually pulled in. If the file is shorter
than expected (truncated/corrupted), `read()` won't throw — it just reads
fewer bytes and sets a fail flag. Checking `gcount()` explicitly catches
that instead of silently continuing with a partially-filled buffer:

```cpp
if (in.gcount() != static_cast<std::streamsize>(count * sizeof(Record)))
    throw std::runtime_error("incomplete file!");
```

---

## 5. Struct padding

`sizeof(Record)` isn't always the sum of its members' sizes — the compiler
may insert padding bytes so each field starts at an address aligned to its
own size requirement. In this exercise's `Record`:

```
uint32_t id;     // 4 bytes, needs 4-byte alignment
float score;     // 4 bytes, needs 4-byte alignment
char name[32];   // 32 bytes, needs 1-byte alignment
```

4 + 4 = 8 (already 4-aligned), then the `char[32]` follows with no
alignment conflict — so `sizeof(Record)` here is exactly 40, with **zero**
padding. Confirmed empirically: writing 3 records produced a 120-byte file
(3 × 40). This won't always be zero for other struct layouts (e.g.
mixing a `char` and a `double` can insert several padding bytes) — worth
checking `sizeof(T)` against the naive sum of field sizes whenever the
exact byte layout matters, such as when a file format needs to be read by
a different compiler/platform later.

---

## 6. Verifying round-trip correctness: `memcmp`, not eyeballing

Printing the reloaded values and visually comparing them only proves the
*values* survived — not that the *exact byte layout* did. A real check
compares raw bytes directly:

```cpp
bool equal = std::memcmp(original, loaded.data(), count * sizeof(Record)) == 0;
```

`memcmp` (from `<cstring>`) compares two memory regions byte-for-byte and
returns `0` only if every byte matches. This is a much stronger guarantee
than "the printed fields looked right."

---

## 7. Gotcha hit while building this: assigning a braced-init-list to an existing object

```cpp
Record *original = new Record[3];
original[0] = {1, 95.5f, "Soumava"};   // compile error!
```

This fails to compile. The reason: `{1, 95.5f, "Soumava"}` on its own is
just a braced-init-list, not yet a `Record`. Declaring **and**
initializing a new object in one step is allowed to use it directly
(aggregate initialization):

```cpp
Record r = {1, 95.5f, "Soumava"};   // OK — this is initialization
```

But *assigning* to an object that already exists goes through normal
operator overload resolution for `operator=`, and a bare braced-init-list
isn't automatically converted into a temporary object to satisfy that —
not for a plain aggregate type with no user-declared constructors.

**Fix:** name the type explicitly, so you're constructing a real temporary
`Record` first, then assigning *that* — a normal, typed value, not a raw
list:

```cpp
original[0] = Record{1, 95.5f, "Soumava"};   // OK — now it's a real Record
```
