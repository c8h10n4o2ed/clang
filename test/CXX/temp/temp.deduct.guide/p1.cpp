// RUN: %clang_cc1 -std=c++1z -fcxx-exceptions -verify %s
// RUN: %clang_cc1 -std=c++1z -fcxx-exceptions -verify %s -DCLASS

#ifdef CLASS
struct Outer {
#endif

template<typename> struct A {};

// Valid forms.
A(int(&)[1]) -> A<int>;
explicit A(int(&)[2]) -> A<int>;

// Declarator pieces are not OK.
*A(int(&)[3]) -> A<int>; // expected-error {{cannot specify any part of a return type in the declaration of a deduction guide}}
&A(int(&)[4]) -> A<int>; // expected-error {{cannot specify any part of a return type in the declaration of a deduction guide}}
A(int(&)[5])[3] -> A<int>;
#ifdef CLASS // FIXME: These diagnostics are both pretty bad.
// expected-error@-2 {{deduction guide declaration without trailing return type}} expected-error@-2 {{array of 'auto'}} expected-error@-2 {{';'}}
#else
// expected-error@-4 {{expected function body after function declarator}}
#endif

// (Pending DR) attributes and parens around the declarator-id are OK.
[[deprecated]] A(int(&)[6]) [[]] -> A<int> [[]];
A [[]] (int(&)[7]) -> A<int>; // FIXME: expected-error 2{{expected}} expected-note {{to match}}
(A)(int(&)[8]) -> A<int>;

// ... but the trailing-return-type is part of the function-declarator as normal
(A(int(&)[9])) -> A<int>;
#ifdef CLASS // FIXME: These diagnostics are both pretty bad.
// expected-error@-2 {{deduction guide declaration without trailing return type}} expected-error@-2 {{';'}}
#else
// expected-error@-4 {{expected function body after function declarator}}
#endif
(A(int(&)[10]) -> A<int>); // expected-error {{trailing return type may not be nested within parentheses}}

// A trailing-return-type is mandatory.
A(int(&)[11]); // expected-error {{deduction guide declaration without trailing return type}}

// No type specifier is permitted; we don't even parse such cases as a deduction-guide.
int A(int) -> A<int>; // expected-error {{function with trailing return type must specify return type 'auto', not 'int'}}
template<typename T> struct B {}; // expected-note {{here}}
auto B(int) -> B<int>; // expected-error {{redefinition of 'B' as different kind of symbol}}

// FIXME: No storage class specifier, function specifier, ...
friend A(int(&)[20]) -> A<int>;
#ifdef CLASS
// expected-error@-2 {{cannot declare a deduction guide as a friend}}
#else
// expected-error@-4 {{'friend' used outside of class}}
#endif
typedef A(int(&)[21]) -> A<int>; // FIXME: Bad diagnostic: expected-error {{typedef name must be an identifier}}
constexpr A(int(&)[22]) -> A<int>;
inline A(int(&)[23]) -> A<int>;
static A(int(&)[24]) -> A<int>;
thread_local A(int(&)[25]) -> A<int>; // expected-error {{'thread_local' is only allowed on variable declarations}}
extern A(int(&)[26]) -> A<int>;
#ifdef CLASS
// expected-error@-2 {{storage class specified for a member}}
#endif
mutable A(int(&)[27]) -> A<int>; // expected-error-re {{{{'mutable' cannot be applied to|illegal storage class on}} function}}
virtual A(int(&)[28]) -> A<int>; // expected-error {{'virtual' can only appear on non-static member functions}}

// FIXME: No definition is allowed.
A(int(&)[30]) -> A<int> {}
A(int(&)[31]) -> A<int> = default; // expected-error {{only special member functions may be defaulted}}
A(int(&)[32]) -> A<int> = delete;
A(int(&)[33]) -> A<int> try {} catch (...) {}

#ifdef CLASS
};
#endif
