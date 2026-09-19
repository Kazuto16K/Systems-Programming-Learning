#pragma once
// Minimal C++17 Result<T, E>. Holds exactly one of a T or an E.
// Migration path: API names match std::expected (has_value, value, error,
// value_or, and_then, transform->map).

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

// Use Result<Unit, E> for "nothing on success".
struct Unit {};

// Tag wrappers: keep variant alternatives distinct even when T == E.
template <class T> struct Ok  { T value; };
template <class E> struct Err { E error; };
template <class T> Ok(T)  -> Ok<std::decay_t<T>>;
template <class E> Err(E) -> Err<std::decay_t<E>>;

// [[nodiscard]] on the class: dropping any returned Result warns.
template <class T, class E>
class [[nodiscard]] Result {
    std::variant<Ok<T>, Err<E>> v_;

    [[noreturn]] static void die(const char* msg) {
        std::fprintf(stderr, "Result: %s\n", msg);
        std::abort();
    }

public:
    using value_type = T;
    using error_type = E;

    // Implicit from Ok{...} / Err{...}; allows Ok{"lit"} -> Result<std::string, E>.
    template <class U, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
    Result(Ok<U> ok) : v_(std::in_place_index<0>, Ok<T>{T(std::move(ok.value))}) {}

    template <class U, std::enable_if_t<std::is_constructible_v<E, U&&>, int> = 0>
    Result(Err<U> er) : v_(std::in_place_index<1>, Err<E>{E(std::move(er.error))}) {}

    bool has_value() const noexcept { return v_.index() == 0; }
    explicit operator bool() const noexcept { return has_value(); }

    // Checked accessors: wrong-state access aborts loudly, never UB.
    T& value() & {
        if (!has_value()) die("value() called on an error Result");
        return std::get<0>(v_).value;
    }
    const T& value() const& {
        if (!has_value()) die("value() called on an error Result");
        return std::get<0>(v_).value;
    }
    T&& value() && {
        if (!has_value()) die("value() called on an error Result");
        return std::move(std::get<0>(v_).value);
    }

    E& error() & {
        if (has_value()) die("error() called on an ok Result");
        return std::get<1>(v_).error;
    }
    const E& error() const& {
        if (has_value()) die("error() called on an ok Result");
        return std::get<1>(v_).error;
    }
    E&& error() && {
        if (has_value()) die("error() called on an ok Result");
        return std::move(std::get<1>(v_).error);
    }

    template <class U>
    T value_or(U&& fallback) const& {
        return has_value() ? std::get<0>(v_).value
                           : static_cast<T>(std::forward<U>(fallback));
    }
    template <class U>
    T value_or(U&& fallback) && {
        return has_value() ? std::move(std::get<0>(v_).value)
                           : static_cast<T>(std::forward<U>(fallback));
    }

    // map: T -> U, error passes through. (std::expected::transform)
    template <class F>
    auto map(F&& f) && {
        using U = std::invoke_result_t<F, T>;
        if (has_value())
            return Result<U, E>(Ok<U>{std::invoke(std::forward<F>(f), std::move(*this).value())});
        return Result<U, E>(Err<E>{std::move(*this).error()});
    }

    // and_then: T -> Result<U, E>, error passes through.
    template <class F>
    auto and_then(F&& f) && {
        using R = std::invoke_result_t<F, T>;
        if (has_value())
            return std::invoke(std::forward<F>(f), std::move(*this).value());
        return R(Err<E>{std::move(*this).error()});
    }
};