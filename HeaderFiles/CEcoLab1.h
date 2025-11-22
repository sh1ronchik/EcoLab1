/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает реализацию компонента CEcoLab1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __C_ECOLAB1_H__
#define __C_ECOLAB1_H__

#include "IEcoLab1.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"


typedef struct CEcoLab1 {
    /* Таблица функций интерфейса IEcoLab1 */
    IEcoLab1VTbl* m_pVTblIEcoLab1;
    
    /* Таблица функций интерфейса IEcoCalculatorX */
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;
    
    /* Таблица функций интерфейса IEcoCalculatorY */
    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;
    
    /* Счетчик ссылок */
    uint32_t m_cRef;
    
    /* Интерфейс для работы с памятью */
    IEcoMemoryAllocator1* m_pIMem;
    
    /* Системный интерфейс */
    IEcoSystem1* m_pISys;
    
    /* Данные экземпляра */
    char_t* m_Name;
    
    /* Агрегирование компонента B */
    IEcoCalculatorX* m_pAggregatedCalcX;
    
    /* Включение компонентов */
    IEcoCalculatorX* m_pContainedCalcX_A;  /* Add */
    IEcoCalculatorX* m_pContainedCalcX_C;  /* Sub */
    IEcoCalculatorY* m_pContainedCalcY_D;  /* Mult */
    IEcoCalculatorY* m_pContainedCalcY_E;  /* Div */
} CEcoLab1, *CEcoLab1Ptr;

/* Инициализация экземпляра */
int16_t ECOCALLMETHOD initCEcoLab1(/*in*/ IEcoLab1Ptr_t me, /* in */ struct IEcoUnknown *pIUnkSystem);

/* Создание экземпляра */
int16_t ECOCALLMETHOD createCEcoLab1(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoLab1** ppIEcoLab1);

/* Удаление */
void ECOCALLMETHOD deleteCEcoLab1(/* in */ IEcoLab1* pIEcoLab1);

#endif /* __C_ECO_LAB1_H__ */