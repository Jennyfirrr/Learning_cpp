#include <cmath>
#include <iostream>

double poly(double a) {
  double b = (5 * a) - 3;
  return b;
}

double perim(double l, double w) {
  double perm = (l * 2) + (w * 2);
  return perm;
}

double disc() {
  double price = 0;
  double discount = 0;
  std::cout << "Please enter the price : ";
  std::cin >> price;
  std::cout << "Please enter a discount: ";
  std::cin >> discount;

  double total = price * (1 - (discount / 100));
  return total;
}

struct mort {
  double principle;
  double rate;
  double years;
};

double monthly(mort &m) {
  std::cout << "Please enter your principle: ";
  std::cin >> m.principle;
  std::cout << "Please enter your rate: ";
  std::cin >> m.rate;
  std::cout << "Please enter how many years: ";
  std::cin >> m.years;
  double i = (m.rate / 100) / 12;
  double n = m.years * 12;
  double j = 1 + i;
  double copy = std::pow(j, n);
  double top = i * copy;
  double bot = copy - 1;
  double form = top / bot;
  double total = m.principle * form;
  return total;
}

struct avghw {
  float avg_exams;
  float avg_hw;
  int attend;
};

bool grader(avghw &a) {
  std::cout << "Please enter the avg homework score: ";
  std::cin >> a.avg_hw;
  std::cout << "Please enter the avg exam score: ";
  std::cin >> a.avg_exams;
  std::cout << "Please enter the attendance: ";
  std::cin >> a.attend;

  if (a.attend < 16 || a.avg_hw < 75 || a.avg_exams < 75) {
    return false;
  } else if (a.avg_hw > 80 || a.avg_exams > 80) {
    return true;
  } else {
    return false;
  }
}

double radian() {
  double rad = 0;
  std::cout << "Please enter a radian value: ";
  std::cin >> rad;
  double degree = (180 / M_PI) * rad;
  return degree;
}

int main() {
  mort m{};
  avghw a{};
  double l = 0;
  double w = 0;
  double num = 0;
  std::cout << "Please enter a number: ";
  std::cin >> num;
  std::cout << poly(num) << std::endl;
  std::cout << "\nPlease enter a length: ";
  std::cin >> l;
  std::cout << "Please enter a width: ";
  std::cin >> w;
  std::cout << perim(l, w) << std::endl;
  std::cout << disc() << std::endl;
  std::cout << monthly(m) << std::endl;
  std::cout << grader(a) << std::endl;
  std::cout << radian() << std::endl;
}
