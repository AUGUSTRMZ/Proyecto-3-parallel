# Proyecto 3 — Suma de Números Primos Paralela
 
Calcula la suma de todos los números primos menores a **5,000,000** usando tres enfoques distintos.
 
**Resultado esperado:** `838,596,693,108`
 
---
 
## Versiones
 
| Versión | Archivo | Tecnología |
|---|---|---|
| Secuencial | `primes_sequential.cpp` | C++ (1 núcleo) |
| Paralela CPU | `primes_threads.cpp` | C++ `std::thread` |
| Paralela GPU | `primes_cuda.cu` | CUDA |
 
---

## Resultados
 
| Versión | Tiempo Promedio | Resultado |
|---|---|---|
| Secuencial | 790.263 ms | 838,596,693,108 |
| C++ Threads (24 hilos) | 88.598 ms | 838,596,693,108 |
| CUDA (32x512 hilos) | 10 ms | 838,596,693,108 |

