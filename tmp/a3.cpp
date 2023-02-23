#include <iostream>

using namespace std;

class ABC {
public:
  ABC(double bb, char cc) : b(bb), c(cc) {}
  double getVal() { return b * c + 4; }

private:
  double b;
  char c;
};

int f() {
  ABC abc(23.4, 'a');
  int a = 0;
  a = 44;
  a = 1234;
  cin >> a;
  if (a >= 5) {
    a = a * 3 + 7;
    return 10;
  } else {
    int b;
    cin >> b;
    a -= b;
    return 20 - abc.getVal();
  }
}

int main() {
  cout << f() << endl;
  return 0;
}
