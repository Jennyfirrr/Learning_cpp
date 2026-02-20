STUDY GUIDE CS203

Go over constructor, [~5 questions] 

public class Dog {
    private int weight;
}

public Dog{
    public Dog() {
    this.weight = 0;
}
}

Everything in java is an Object, except primitives

all objects are Objects, difference between object and Objects

Object mutate 
public void SwapX (Point a, Point b) {
    int temp = a.x;
    a.x = b.x;
    b.x = temp;
}

Encapusulation, hiding implementation details and providing methods for data acess and behavior
Essentially Libraries

Less tech debt in theory, make functions reusable is the main idea.

design by contract, see integration contracts in FoxML

Class Hierarchy

Getters/setters 

object superclass

Object's Public Interface (API)

public api defins the minimum functionality and state that all objects have

Overriding mthods, You can override a method from a parent class, you can change what toString() does, you are changing implementation, not the interface.

method design, design functions that are EITHER accessor or mutator, not both, decreases techdebt, increases maintainability

local variables are declared in the body of a method.

local variables, parameters, instance variables

go over ownership

Abstraction

minimize dependencies and circular imports
Cash register depends on coin, but coin doesnt depend on cash register

Mutator -> modify [side effect]

A method should not modify its parameter values

static variables and non static.



