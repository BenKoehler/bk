#include <iostream>

#include <bk/CopyablePIMPL>

//------------------------------------------------------------------------------------------------------
// class Foo declaration
//------------------------------------------------------------------------------------------------------
class Foo
{
    class Impl;
    bk::cpimpl<Impl> _pdata;

  public:
    Foo();
    Foo(const Foo&);
    Foo(Foo&&) noexcept;
    Foo(int y);
    ~Foo();

    int x() const;
    int& x();

    [[maybe_unused]] Foo& operator=(const Foo&);
    [[maybe_unused]] Foo& operator=(Foo&&) noexcept;
}; // class Foo

//------------------------------------------------------------------------------------------------------
// class Foo definition
//------------------------------------------------------------------------------------------------------
struct Foo::Impl
{
    int x;

    Impl() : x(1337) { /* do nothing */ }
    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;
    Impl(int y) : x(y) { /* do nothing */ }
    ~Impl() = default;
    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

Foo::Foo() = default;
Foo::Foo(const Foo&) = default;
Foo::Foo(Foo&&) noexcept = default;
Foo::Foo(int y) : _pdata(y) { /* do nothing */ }
Foo::~Foo() = default;
Foo& Foo::operator=(const Foo&) = default;
Foo& Foo::operator=(Foo&&) noexcept = default;

int Foo::x() const { return _pdata->x; }
int& Foo::x() { return _pdata->x; };

//------------------------------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------------------------------
int main(int /*argc*/, char** /*argv*/)
{
    Foo f(5);
    std::cout << "f.x = " << f.x() << std::endl;

    Foo g(f);

    Foo h(std::move(g));
    h = f;

    Foo i;

    return EXIT_SUCCESS;
}

