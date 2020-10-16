#include <sugarpp/types/types.hpp>
#include <sugarpp/io/io.hpp>

using namespace SugarPP;

int main()
{
    auto s = to_string(123.0);
    print(s);
}
