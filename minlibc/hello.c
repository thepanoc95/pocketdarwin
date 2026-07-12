#include <unistd.h>

int main()
{
    write(1, "Hello from minlibc!\n", 20);
    return 0;
}
