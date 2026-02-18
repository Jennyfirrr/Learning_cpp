import math as m

def myName():
    return "Jennifer Lewis"

def myBlazerID():
    return "jtlewis"

print("My name is =", myName(), " and my BlazerID is =", myBlazerID())

def sumDigitsPrime(n):
    total = 0
    digits = []

    while(n > 0):
        x = n % 10
        digits.append(x)
        n //= 10
    
    for y in digits:
        total += y

    check = 1 + round(m.sqrt(total))
    checker = 2

    while (checker <= check):
        if (total % checker == 0):
            return "Not prime"
        else:
            checker += 1

    return "Prime"

def fileCountAdvisors():


