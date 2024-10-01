#include <iostream>
class A{
public:
    int * d;
    int x=0;
    A(int c){
        x = c;
        d = new int[3];
        d[0] = 1;
        d[1] = 2;
        d[2] = 3;
    }
    A( A&& other){
        x = other.x;
        d = other.d;
        other.d = nullptr;
    }

    A& operator=(A&& other) noexcept {
        std::cout << "Move assignment operator called\n";
        if (this == &other) return *this;
        delete[] d;
        d = other.d;     // Transfer ownership
        other.d = nullptr; // Nullify the source to prevent double deletion
        x=other.x;
        return *this;
    }

    ~A(){

        printf("called %d\n", x);
        delete [] d;
    }
};
int main() {
    // Write C++ code here
    class A a(0);
    a=std::move(a);
    printf("a reinitialized\n");
    return 0;
}