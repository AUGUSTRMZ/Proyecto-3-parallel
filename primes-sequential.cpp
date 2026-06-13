/*
Este archivo implementa la version
secuencial del algoritmo que calcula la suma de todos los numeros primos menores a 5,000,000

Resultado esperado : 836, 596, 693, 108

*/

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

// limite superior verificamos todos los enteros en [2, LIMIT]
#define LIMIT 5000000

// numero de repeticiones para calcular un timepo promedio compfiable
#define N 10

/*
esPrimo: determina si un numero entero n es primo usando division por tentativa hasta ceil(sqrt(n))

1. Si n <2, no es primo
2. Si n % i == 0, n es divisible por lo tanto NO es primo
3. Si no se encontro ningun divisor, n SI es primo
*/

bool esPrimo(int n)
{
    if (n < 2)
    {
        return false;
    }

    // calculo el techo del sqrt. El +1 nos da el techo cuando trunca con int
    // suamos < en el ciclo para que i nunca llegue a ese valor para evitar falsos negativos
    int limit = (int)sqrt((double)n) + 1;
    for (int i = 2; i < limit; i++)
    {
        // si i divide exactamente a n, no es primo
        if (n % i == 0)
        {
            return false;
        }
    }
    // ningun divisor encontrado n es primo
    return true;
}

int main(int argc, char *argv[])
{
    // lonlong porque el resultado esperado es muy grande
    long long result;

    // para medir el tiempo de ejecucion
    high_resolution_clock::time_point startTime, endTime;
    double timeElapsed;

    cout << "SUMA SECUENCIAL DE NUMEROS PRIMOS ( LIMITE = " << LIMIT << ")" << endl;
    cout << "INICIANDO..." << endl;
    timeElapsed = 0;

    // N veces para obtener un tiempo promedio estable
    for (int j = 0; j < N; j++)
    {
        // Reiniciar el acumulador al inicio de cada repeticion
        result = 0;

        // iniciar el cronometro DESPUES de cualquier preparacion
        startTime = high_resolution_clock::now();

        // revisar cada candidato en [2, LIMIT]
        for (int i = 2; i < LIMIT; i++)
        {
            if (esPrimo(i))
            {
                result += i; // acumulamos los primos encontrados
            }
        }
        // parar el cronometro despues del computo
        endTime = high_resolution_clock::now();

        // acumula el tiempo que paso en mili
        timeElapsed +=
            duration<double, milli>(endTime - startTime).count();
    }

    cout << "resultado  = " << result << endl;
    cout << "tiempo promedio  = " << fixed << setprecision(3) << (timeElapsed / N) << "ms/n";

    return 0;
}