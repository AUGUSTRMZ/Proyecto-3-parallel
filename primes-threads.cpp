/*
Este archivo implementa la version paralela del algoritmo que calcula la suma de todos los numeros primos
menores a 5,000,000
*/
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;
using namespace std::chrono;

// limite superior verificamos todos los enteros en [2, LIMIT]
#define LIMIT 5000000

// numero de repeticiones para calcular un timepo promedio compfiable
#define N 10

/*
Detecta el numero de nubleso disponibles en el sistema
Usa todos los nucleos al maximo para aprovechar el procesador
*/

#define THREADS std::thread::hardware_concurrency()

/*
Retorna true si es primo, Division hasta sqrt(n)
*/

bool esPrimo(int n)
{
    if (n < 2)
    {
        return false;
    }

    int limit = (int)sqrt((double)n) + 1;
    for (int i = 2; i < limit; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

/*
Itera sobre el subrango [start, end] asignado al hilo
llama a esPrimo para cada candidato y acumula la suma de
los primos encontrados en el rango

el resultsado se guarda en results para que el hilo principal lo pueda leer despues
del join


start primero entero a revisar
end limite superior del rango
results suma de los primos enocntrados entre el rango [start, end]
*/

void sum_primos_en_rango(int start, int end, long long &results)
{
    // acumulador sin usar memoria compartida
    long long local = 0;

    for (int i = start; i < end; i++)
    {
        if (esPrimo(i))
        {
            local += i;
        }
    }
    results = local;
}

int main(int argc, char *argv[])
{
    high_resolution_clock::time_point startTime, endTime;
    double timeElapsed;
    /*
        dividir el rango [2, LIMIT] en THREADS bloques contiguos
        si LIMIT no es divisible entre THREADS, los primos hilos reciben un elemnento exra para no dejar
        huevos
    */

    int block_size = LIMIT / THREADS;
    int remainder = LIMIT % THREADS;

    // un thread por nucleo del cpu
    thread threads[THREADS];

    // cada hilo escribe su suma parcial aqui
    long long results[THREADS];

    cout << "Suma de primos" << endl;
    cout << "Hilos = " << THREADS << endl;
    cout << "Iniciando..." << endl;

    timeElapsed = 0;
    long long result = 0;

    for (int j = 0; j < N; j++)
    {
        startTime = high_resolution_clock::now();
        // lanzar los hilos
        int start = 0;
        for (int i = 0; i < (int)THREADS; i++)
        {
            // los hilos 0...(remainder-1) reciben un entero extra para cubrir todo el rango [0, LIMIT]
            int end = start + block_size + ((i < remainder) ? 1 : 0);
            // std::ref se usa porque sum_primos_en_rango recibe el ultimo argumento por referencia
            // esto para no copiar la variagble en lugar de crear un alias
            threads[i] = thread(sum_primos_en_rango, start, end, std::ref(results[i]));
            start = end;
        }
        // espera a que los hilos terminen
        result = 0;
        for (int i = 0; i < (int)THREADS; i++)
        {
            // bloqueo hasta que el hilo i haya terminado
            threads[i].join();
            // solo es seguro leer results[i] despues de join()
            result += results[i];
        }
        endTime = high_resolution_clock::now();
        timeElapsed += duration<double, milli>(endTime - startTime).count();
    }
    cout << "resultado =" << result << endl;
    cout << "tiempo promedio =" << fixed << setprecision(3) << (timeElapsed / N) << "ms" << endl;

    return 0;
}