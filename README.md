# Техническое задание

## Управляющее устройство

Принимает команды от пользователя из stdin вида:

```
команда <u1> <u2> <u3>
addri abc 123 bcd
addii 123 465
```

Если команда является операцией над целыми числами, то управление передается а АЛУ, если команда связана с изменением БД, то, соответственно блоку, который отвечает за БД, если команда связана с работой над ЧПТ (синусы/косинусы), то в FPU.

## FPU

Работает с ЧПТ. Содержит функционал, способный выполнять такие операции, как сложение, вычитание, деление, сравнение двух ЧПТ, а также применение функции sin к данному ЧПТ.

### Сигнатуры функций:

``` c
/// @brief Adds two floating point numbers together
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_add(double a, double b, double *result);

/// @brief Substructs second floating point number from second
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_sub(double a, double b, double *result);

/// @brief Divides first floating point number by second
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result (floating point number)
/// @return Exit code
status_t fpu_div(double a, double b, double *result);

/// @brief Applies sin func to the floating point number
/// @param a floating point number (arg)
/// @param result func result (floating point number)
/// @return Exit code
status_t fpu_sin(double a, double *result);

/// @brief Checks which one floating point number is bigger 
/// @param a first floating point number
/// @param b second floating point number
/// @param result operation result: -1, if first < second. 0, if first == second. 1, if first > second (integer)
/// @return Exit code
status_t fpu_cmp(double a, double b, int *result);
```

## База данных

Схема взаимодействия с БД:

```
УУ -> КЭШ -> БД
```

КЭШ хранит 5 'последних' элементов. Если запрашиваемый элемент хранится в кэше, то обращения к БД не происходит, значение берется из него непосредственно (КЕШ хранит пары ключ-значение). Если какой-либо элемент необходимо записать в БД, то он сначала помещается в КЭШ, в саму БД элемент добавляется только в момент, когда он вытесняется из КЭШа или когда работа завершается.

# Технические особенности проекта

Архитектура проекта организована "по Бекасову" (структурное программирование, каждая функция возвращает статус своего выполнения).
В проекте для обозначения статуса выполнения программы или подпрограммы используется тип данных ```status_t``` (errors.h):

```c
#ifndef __ERRORS_H__
#define __ERRORS_H__

typedef enum 
{
    SUCCESS_CODE,
    ERR_IO,
    ERR_MEM,
    ERR_RANGE,
    ERR_NOT_FOUND
} status_t;

#endif
```

