/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "IEcoCalculatorY.h"
#include "IEcoCalculatorX.h"

#if defined(_MSC_VER)
 #define CDECL __cdecl
#else
 #define CDECL
#endif

/*
 *
 * <сводка>
 *   Простая LCG для детерминированных "случайных" значений
 * </сводка>
 *
 * <описание>
 *   Линейный конгруэнтный генератор — используется внутри init-функций,
 *   чтобы не зависеть от внешних заголовков/реализаций rand/srand.
 * </описание>
 *
 */
static unsigned long lcg_seed = 1;
static void lcg_srand(unsigned long s) { lcg_seed = s; }
static int lcg_rand(void) {
    lcg_seed = lcg_seed * 1103515245UL + 12345UL;
    return (int)((lcg_seed >> 16) & 0x7FFF);
}

/*
 *
 * <сводка>
 *   Инициализация массива int32_t
 * </сводка>
 *
 * <описание>
 *   Выделяет массив int32_t размера size через pIMem и заполняет некоторыми значениями.
 *   Возвращает указатель на выделенный массив (int32_t*).
 * </описание>
 *
 */
void *initIntArr(IEcoMemoryAllocator1 *pIMem, size_t size) {
    int32_t *arr = 0;
    size_t i;
    if (pIMem == 0) return 0;
    arr = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(int32_t)));
    if (arr == 0) return 0;
    for (i = 0; i < size; ++i) {
        arr[i] = (int32_t)(((i * 37) % 21) - 10);
    }
    return arr;
}

/*
 *
 * <сводка>
 *   Вспомогательные функции для вывода массивов
 * </сводка>
 *
 */
static void printIntArr(const int32_t *arr, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        if (i) printf(", ");
        printf("%d", arr[i]);
    }
    printf("]\n");
}

/* comparators for qsort */
static int CDECL compareIntQ(const void *a, const void *b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}


/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
	/* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
	/* Указатель на интерфейс работы с системной шиной */
    IEcoInterfaceBus1* pIBus = 0;
	/* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
	/* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;

	IEcoCalculatorX* pIX = 0;
	IEcoCalculatorY* pIY = 0;


	/* Проверка и создание системного интрефейса */
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
    if (result != 0 || pISys == 0) {
        printf("ERROR: failed to obtain IEcoSystem1\n");
		/* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

	/* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
		/* Освобождение в случае ошибки */
        printf("ERROR: failed to obtain IEcoInterfaceBus1\n");
        goto Release;
    }


#ifdef ECO_LIB
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0) {
        printf("ERROR: component registration failed\n");
        goto Release;
    }
#endif

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    if (result != 0 || pIMem == 0) {
        printf("ERROR: failed to obtain IEcoMemoryAllocator1\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        printf("ERROR: failed to obtain IEcoLab1\n");
        goto Release;
    }
	

   /* ---------- TEST 1: int32_t ---------- */
    { 
        int32_t *src = 0;
        int32_t *arr2 = 0;
        size_t n = 100000;
        size_t i;
        int ok_csort = 0;
        int ok_qsort = 0;
        clock_t t0, t1, tq0, tq1;
        double elapsed_csort, elapsed_qsort;

		printf("=== TEST LAB1 ===\n\n");

        src = (int32_t*)initIntArr(pIMem, n);
        if (src == 0) { printf("ERROR: initIntArr failed\n"); goto Release; }

        /* копия для qsort (делаем перед сортировкой pIEcoLab1) */
        arr2 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (arr2 == 0) { pIMem->pVTbl->Free(pIMem, src); printf("ERROR: alloc arr2 failed\n"); goto Release; }
        for (i = 0; i < n; ++i) arr2[i] = src[i];

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortInt(pIEcoLab1, src, n);
        t1 = clock();
        elapsed_csort = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST INT: csortInt returned error %d\n", result);
        } else {
            ok_csort = 1;
            for (i = 1; i < n; ++i) {
                if (src[i-1] > src[i]) { ok_csort = 0; break; }
            }
            printf("TEST INT: csort %s (time = %.6f s)\n", ok_csort ? "PASS" : "FAIL", elapsed_csort);
        }

        /* qsort on arr2 */
        tq0 = clock();
        qsort(arr2, n, sizeof(int32_t), compareIntQ);
        tq1 = clock();
        elapsed_qsort = (double)(tq1 - tq0) / (double)CLOCKS_PER_SEC;

        ok_qsort = 1;
        for (i = 1; i < n; ++i) {
            if (arr2[i-1] > arr2[i]) { ok_qsort = 0; break; }
        }
        printf("TEST INT: qsort %s (time = %.6f s)\n\n", ok_qsort ? "PASS" : "FAIL", elapsed_qsort);

        pIMem->pVTbl->Free(pIMem, src);
        pIMem->pVTbl->Free(pIMem, arr2);
    }

	
	
	{
		IEcoLab1* labComp = 0;
		IEcoCalculatorX* ix = 0;
		IEcoCalculatorY* iy = 0;
		IEcoLab1* labBack = 0;
		int16_t rc = -1;

		printf("=== TEST LAB2 ===\n\n");
		printf("\n=== LAB2: Practical arithmetic & property tests ===\n\n");

		/* ---- Case A ---- */
		labComp = 0; ix = 0; iy = 0;
		rc = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&labComp);
		if (rc == 0 && labComp) {
			printf("Case A:\n");
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorX, (void**)&ix);
			if (rc == 0 && ix) {
				printf("  6 + 7 = %d\n", ix->pVTbl->Addition(ix, 6, 7));
				printf("  14 - 5 = %d\n", ix->pVTbl->Subtraction(ix, 14, 5));
				ix->pVTbl->Release(ix); ix = 0;
			} else {
				printf("  IX: unavailable\n");
			}
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorY, (void**)&iy);
			if (rc == 0 && iy) {
				printf("  5 * 4 = %d\n", iy->pVTbl->Multiplication(iy, 5, 4));
				printf("  9 / 3 = %d\n", iy->pVTbl->Division(iy, 9, 3));
				iy->pVTbl->Release(iy); iy = 0;
			} else {
				printf("  IY: unavailable\n");
			}
			labComp->pVTbl->Release(labComp); labComp = 0;
		} else {
			printf("Case A: cannot obtain IEcoLab1 (rc=%d)\n", rc);
		}

		/* ---- Case B ---- */
		labComp = 0; ix = 0; iy = 0;
		rc = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&labComp);
		if (rc == 0 && labComp) {
			printf("\nCase B:\n");
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorX, (void**)&ix);
			if (rc == 0 && ix) {
				printf("  2 + 11 = %d\n", ix->pVTbl->Addition(ix, 2, 11));
				printf("  -2 - (-6) = %d\n", ix->pVTbl->Subtraction(ix, -2, -6));
				ix->pVTbl->Release(ix); ix = 0;
			}
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorY, (void**)&iy);
			if (rc == 0 && iy) {
				printf("  8 * 6 = %d\n", iy->pVTbl->Multiplication(iy, 8, 6));
				printf("  18 / 3 = %d\n", iy->pVTbl->Division(iy, 18, 3));
				iy->pVTbl->Release(iy); iy = 0;
			}
			labComp->pVTbl->Release(labComp); labComp = 0;
		}

		/* ---- Case C ---- */
		labComp = 0; ix = 0; iy = 0;
		rc = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&labComp);
		if (rc == 0 && labComp) {
			printf("\nCase C:\n");
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorX, (void**)&ix);
			if (rc == 0 && ix) {
				printf("  202 + 808 = %d\n", ix->pVTbl->Addition(ix, 202, 808));
				printf("  1000 - 7 = %d\n", ix->pVTbl->Subtraction(ix, 1000, 7));
				ix->pVTbl->Release(ix); ix = 0;
			}
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorY, (void**)&iy);
			if (rc == 0 && iy) {
				printf("  12 * 12 = %d\n", iy->pVTbl->Multiplication(iy, 12, 12));
				printf("  125 / 5 = %d\n", iy->pVTbl->Division(iy, 125, 5));
				iy->pVTbl->Release(iy); iy = 0;
			}
			labComp->pVTbl->Release(labComp); labComp = 0;
		}

		/* ---- Property test 1: IX -> IY -> ILab1 ---- */
		labComp = 0; ix = 0; iy = 0; labBack = 0;
		rc = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&labComp);
		if (rc == 0 && labComp) {
			printf("\nProperty test 1: IX -> IY -> ILab1\n");
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorX, (void**)&ix);
			if (rc == 0 && ix) {
				rc = ix->pVTbl->QueryInterface(ix, &IID_IEcoCalculatorY, (void**)&iy);
				if (rc == 0 && iy) {
					rc = iy->pVTbl->QueryInterface(iy, &IID_IEcoLab1, (void**)&labBack);
					if (rc == 0 && labBack) {
						printf("  IX -> IY -> ILab1 : OK (labBack=%p)\n", (void*)labBack);
						labBack->pVTbl->Release(labBack); labBack = 0;
					} else {
						printf("  IX -> IY -> ILab1 : FAILED (no ILab1)\n");
					}
					iy->pVTbl->Release(iy); iy = 0;
				} else {
					printf("  IX -> IY : FAILED (no IY)\n");
				}
				ix->pVTbl->Release(ix); ix = 0;
			} else {
				printf("  IX not available (property test skipped)\n");
			}
			labComp->pVTbl->Release(labComp); labComp = 0;
		}

		/* ---- Property test 2: IY -> IX -> ILab1 ---- */
		labComp = 0; ix = 0; iy = 0; labBack = 0;
		rc = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**)&labComp);
		if (rc == 0 && labComp) {
			printf("\nProperty test 2: IY -> IX -> ILab1\n");
			rc = labComp->pVTbl->QueryInterface(labComp, &IID_IEcoCalculatorY, (void**)&iy);
			if (rc == 0 && iy) {
				rc = iy->pVTbl->QueryInterface(iy, &IID_IEcoCalculatorX, (void**)&ix);
				if (rc == 0 && ix) {
					rc = ix->pVTbl->QueryInterface(ix, &IID_IEcoLab1, (void**)&labBack);
					if (rc == 0 && labBack) {
						printf("  IY -> IX -> ILab1 : OK (labBack=%p)\n", (void*)labBack);
						labBack->pVTbl->Release(labBack); labBack = 0;
					} else {
						printf("  IY -> IX -> ILab1 : FAILED (no ILab1)\n");
					}
					ix->pVTbl->Release(ix); ix = 0;
				} else {
					printf("  IY -> IX : FAILED (no IX)\n");
				}
				iy->pVTbl->Release(iy); iy = 0;
			} else {
				printf("  IY not available (property test skipped)\n");
			}
			labComp->pVTbl->Release(labComp); labComp = 0;
		}

		printf("\n=== End of tests ===\n\n");
	}

Release:

	/* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);

	/* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);

	/* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);

	/* Освобождение системного интерфейса */
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    return result;
}
