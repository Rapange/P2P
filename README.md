# P2P

Para compilar el main.cpp:
g++ main.cpp cpeer.cpp cpeer.h -o main -lpthread -std=c++11

Para compilar el server.cpp:
g++ server.cpp -o server -std=c++11 -lpthread

Primero correr el server con ./server
Luego correr el main con ./main

Les va a pedir tres puertos:
Para el primer main (primer peer) que corran pongan los siguientes puertos:
40000 40001 40002

Para el segundo main que corran coloquen puertos distintos a los tres de arriba, todo esto con el fin para poder probar en su misma laptop el programa (como tienen la misma IP, solo se pueden diferenciar por los puertos y hasta ahora cada peer solo puede comunicarse con el primer peer por el problema de los puertos).

Ya para probar en distintas laptops, todos van a tener los mismos puertos.
