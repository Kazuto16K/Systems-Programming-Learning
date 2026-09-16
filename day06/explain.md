# Day 6 — Hash Table: How `hash.cpp` Works

## Design note first: separate chaining, not open addressing

The exercise spec called for open-addressing with linear probing. What you
built instead — `std::vector<std::list<std::pair<K,V>>>` — is **separate
chaining**: the other classic collision-resolution strategy. It's a fully
legitimate hash table design (this is how `std::unordered_map` itself
actually works internally), just a different one than assigned. Worth
knowing both exist; see the collision section below for how chaining
handles what linear probing would otherwise need tombstones for.

---

## What each function does

### `hashKey`
```cpp
size_t hashKey(const K& key) const {
    return std::hash<K>{}(key) % buckets.size();
}
```
Turns a key into a bucket index. `std::hash<K>{}(key)` calls the standard
library's hash function for type `K` (for `std::string`, this hashes the
characters into a `size_t`) — you're not writing the hash algorithm itself,
you're writing everything *around* it: how that raw hash number becomes an
actual array index (`% buckets.size()`), and everything that happens once
you have that index (collision handling, resizing). That division of
labor is deliberate and matches the exercise's intent — the interesting
systems-programming content is the table logic, not reinventing string
hashing.

### `rehash`
```cpp
void rehash(){
    std::vector<std::list<std::pair<K,V>>> oldBuckets = std::move(buckets);
    buckets.assign(oldBuckets.size()*2, {});
    count = 0;
    for (auto& bucket: oldBuckets)
        for (auto& [k,v]: bucket)
            insert(k,v);
}
```
Grows the table. See **"Why rehash is required"** below for the *why* —
mechanically, it: (1) takes ownership of the old bucket array via `std::move`
(cheap — steals the vector's internal pointer rather than copying every
list), (2) replaces `buckets` with a new, empty vector twice the size,
(3) resets `count` to 0 since nothing has been re-added yet, then
(4) walks every old bucket and reinserts every entry through the normal
`insert()` path — which recomputes each key's index against the **new**
(bigger) `buckets.size()`, since the old index is meaningless once the
modulus changed.

### `insert`
```cpp
void insert(const K& key, const V& value){
    double loadFactor = static_cast<double>(count+1)/buckets.size();
    if (loadFactor > MAX_LOAD_FACTOR) rehash();
    size_t idx = hashKey(key);
    for (auto& [k,v]: buckets[idx])
        if (k == key) { v = value; return; }   // key exists: update
    buckets[idx].emplace_back(key, value);       // key new: append
    count += 1;
}
```
Checks *before* inserting whether adding one more entry would push the
load factor over the threshold, and grows the table first if so. Then
walks the target bucket's list: if the key's already there, update its
value in place; otherwise append a new `(key, value)` pair to that
bucket's list and bump `count`.

### `find` / `contains`
```cpp
bool find(const K& key, V& outValue) const {
    size_t idx = hashKey(key);
    for (const auto& [k, v] : buckets[idx])
        if (k == key) { outValue = v; return true; }
    return false;
}
```
Computes the bucket index, then linearly scans *just that bucket's list*
(not the whole table) for a matching key. `contains` is `find` with the
value discarded — existence check only.

### `remove`
Same idea: find the key's bucket, scan its list, and if found, erase that
node from the `std::list` and decrement `count`.

### `operator[]`
Same shape as `insert`, but instead of taking a value, it returns a
*reference* to the slot so the caller can read or write it directly —
`wordCount[word]++` works because `operator[]` finds-or-creates the entry
and hands back a real `int&` into the bucket's list, which `++` then
mutates. This is the same contract `std::map`/`std::unordered_map`'s
`operator[]` has.

---

## Why `std::vector<std::list<std::pair<K,V>>>`, and what a "bucket" is

A **bucket** is one slot in the table's array — in this design, each
bucket is a `std::list` that can hold *any number* of entries (zero, one,
or many). The outer `std::vector` is the fixed-size array of buckets
itself; its length is the table's current capacity.

Why this shape specifically: `hashKey(key)` maps a key down to one bucket
index, but multiple different keys can map to the *same* bucket (a
collision — see below). Separate chaining's answer to that is: don't try
to force every key into its own unique array slot — instead, let each
slot hold a small list of *all* the keys that happened to land there.
`std::pair<K,V>` is what actually lives in each list node — the key next
to its value, since you need both to answer "is this the key I'm looking
for, and if so, what's its value."

---

## How collisions are handled here

A collision is when `hashKey(keyA) == hashKey(keyB)` for two different
keys. With separate chaining, a collision is trivial to handle: both keys
just live in the *same bucket's list*, as two separate list nodes. There's
no need to find them a different slot (which is what open addressing
would do instead, via linear probing). The cost of a collision, in this
design, is simply that `find`/`insert`/`remove` on that bucket now has to
scan past more than one entry before finding (or ruling out) the key it
wants — still fast as long as buckets stay short on average, which is
exactly what keeping the load factor low (via resizing) is for.

This is also why chaining doesn't need the tombstone trick that linear
probing requires for correct deletion: `remove()` can just erase the
node straight out of the bucket's `std::list` (`bucket.erase(it)`), and
every other key's bucket is completely untouched — there's no "probe
chain" for a hole to break, since each key's home is simply its own
bucket's list, independent of every other bucket.

---

## Why `rehash` is required

As more keys get inserted, buckets accumulate more entries each — the
table's **load factor** (`count / buckets.size()`) rises. Once buckets
start holding several entries each instead of ~0–1, `find`/`insert` degrade
from "compute one index, check ~1 entry" toward "compute one index, scan a
long list" — approaching the O(n) linear-scan behavior a hash table exists
to avoid. `rehash()` counters this by doubling `buckets.size()` once the
load factor crosses `MAX_LOAD_FACTOR` (0.75 here), which on average halves
how many entries land in each bucket, keeping lookups fast. The old
entries *must* be moved into new bucket positions (not just copied as-is)
because `hashKey` divides by `buckets.size()` — once that size changes,
practically every key's correct bucket index changes too.

---

## What `explicit` does on the constructor

```cpp
explicit HashTable(size_t initialBuckets = 16)
    : buckets(initialBuckets), count(0){}
```

Without `explicit`, a constructor callable with a single argument doubles
as an **implicit conversion**. That means anywhere a `HashTable<K,V>` is
expected, the compiler would be allowed to silently convert a bare
`size_t` into one on your behalf — e.g. a function `void f(HashTable<std::string,int> t)`
could be called as `f(64)`, and the compiler would quietly construct a
64-bucket `HashTable` from the integer `64` without you asking it to. This
is rarely what you actually want and is a classic source of confusing
bugs (passing a plain number somewhere and getting a full object
construction nobody intended). `explicit` disables that implicit-conversion
path — the constructor can still be called directly
(`HashTable<std::string,int> t(64);`), just not via a silent one-argument
conversion. Good default instinct: mark any single-argument constructor
`explicit` unless you specifically want it to double as an implicit
conversion.
