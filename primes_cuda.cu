/*
Este archivo implementa la version paralela del algoritmo
que calcula la suma de los numeros primos menores a 5,000,000 usando CUDA

*/

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;
using namespace std::chrono;

// limite superior verificamos todos los enteros en [2, LIMIT]
#define LIMIT 5000000

// numero de repeticiones para calcular un timepo promedio compfiable
#define N 10

// configuracion del grid segun lo indicado (32 x 512)
// 512 hilos por bloque
#define THREADS 512
#define BLOCKS 32

/*
esPrimo

__device__ indica que solo puede ser llamada
desde codigo de GPU

Misma logica que las versiones anteriores

n Numero candidato a evaluar
return  si es primo, 0 si no lo es
*/

__device__ int esPrimo(int n)
{
    if (n < 2)
    {
        return 0;
    }
    int limit = (int)sqrtf((float)n) + 1;
    for (int i = 2; i < limit; i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

/*
sumar_primos

cada hilo procesa un conjunto intercalado de enteros

Despues de la acumulacion, el resultado se guarda en memoria compartida
y una reduccion paralela colapsa los THREADS
valores del bloque a uno solo, guardado en results

results , arreglo de salida de longitud BLOCKS, results [b] contiene la suma de primos encontradas por el bloque b

*/

__global__ void sumar_primos(long long *results)
{
    // Memoria compartida (una ranura por hilo)
    __shared__ long long cache[THREADS];

    // Indice global del hilo e indice dentro de la memoria compartida
    int tid = threadIdx.x + (blockIdx.x * blockDim.x);
    int cacheIndex = threadIdx.x;

    // cada hilo acumula los primos de su subconjunto intercalado
    long long local = 0;
    while (tid < LIMIT)
    {
        if (esPrimo(tid))
        {
            local += tid;
        }
        // Avanzar al siguiente entero asignado a este hilo
        tid += blockDim.x * gridDim.x;
    }
    // guarda la suma local en memoria compartida
    cache[cacheIndex] = local;

    // barrera de sincronizacion
    /*
    todos los hilos del bloque deben haber escrito en cache antes de continuar
    con la reduccion
    */
    __syncthreads();

    /*
    En cada iteracion se divide el numero de hilos activos a la mitad
    Cada hilo activo suma su valor con el del hilo en el offset
    */
    int i = blockDim.x / 2;
    while (i > 0)
    {
        if (cacheIndex < i)
        {
            cache[cacheIndex] += cache[cacheIndex + i];
        }
        // debe esperar que todas las sumas del paso actual sean visibles para todos los hilos
        __syncthreads();
        i /= 2;
    }

    // solo el hilo 0 de cada bloque escribe el resultado final del bloque de memoria
    if (cacheIndex == 0)
    {
        results[blockIdx.x] = cache[0];
    }
}

int main(int argc, char *argv[])
{
    // arreglo para recolectar las sumas parciales por bloque
    long long *h_results = new long long[BLOCKS];

    // arreglo en el GPU donde el kernel escribe
    long long *d_results;

    // variables para medir el tiempo
    high_resolution_clock::time_point startTime, endTime;
    double timeElapsed;

    // reserva de memoria en la GPU para BLOCKS suams parciales
    // *cudaMalloc* == new/malloc en GPU
    cudaMalloc((void **)&d_results, BLOCKS * sizeof(long long));

    cout << "Suma de primos paralela - CUDA" << endl;
    cout << "Grid: " << BLOCKS << "bloque x" << THREADS << "hilos = " << (BLOCKS * THREADS) << "hilos de GPU" << endl;
    cout << "Iniciando" << endl;

    timeElapsed = 0;

    // repetimos N veces para obtener un tiempo promedio estable
    for (int j = 0; j < N; j++)
    {
        startTime = high_resolution_clock::now();
        // lanza el kernel
        sumar_primos<<<BLOCKS, THREADS>>>(d_results);

        // host espera a que la GPU haya terminado el trabajo pendiente
        cudaDeviceSynchronize();

        endTime = high_resolution_clock::now();
        timeElapsed += duration<double, milli>(endTime - startTime).count();
    }

    // copia los resultados de GPU a CPU
    // indica la direccion
    cudaMemcpy(h_results, d_results, BLOCKS * sizeof(long long), cudaMemcpyDeviceToHost);

    // suma las BLOCKS  sumas parciales
    long long total = 0;
    for (int i = 0; i < BLOCKS; i++)
    {
        total += h_results[i];
    }

    cout << "resultado =" << total << endl;
    cout << "tiempo promedio =" << fixed << setprecision(3) << (timeElapsed / N) << "ms\n"
         << endl;

    // libera la memoria
    cudaFree(d_results);
    delete[] h_results;

    return 0;
}