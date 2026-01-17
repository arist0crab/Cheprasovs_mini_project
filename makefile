# Компилятор
CC := gcc 

# Создаем необходимые папки если их нет
$(shell mkdir -p out)
$(shell mkdir -p lib)

# Опции компиляции
CFLAGS := -std=c99 -Wall -Werror -Wextra -Wpedantic -Wvla -c -I inc
UFLAGS := -lrt -lcheck -lpthread -lm
LDFLAGS := -lm -lsqlite3

# Имя статической библиотеки
LIB_NAME := libdb.a
LIB_PATH := lib/$(LIB_NAME)

# Общие объектные файлы (исключаем main.o)
OBJS_WITH_MAIN := $(patsubst src/%.c, out/%.o, $(wildcard src/*.c))
OBJS := $(filter-out out/main.o, $(OBJS_WITH_MAIN))

# Библиотечные объектные файлы (исключаем main.o)
LIB_OBJS := $(filter-out out/main.o, $(OBJS_WITH_MAIN))

# $^ - список зависимостей
# $@ - имя цели
# $< - первая зависимость

# Основной исполняемый файл
app.exe: $(OBJS) out/main.o
	$(CC) -o $@ $^ $(LDFLAGS)

# Создание статической библиотеки
$(LIB_PATH) : $(LIB_OBJS)
	ar rcs $@ $^
	@echo "Static library created: $@"

# Правило для создания библиотеки
lib : $(LIB_PATH)

# Сценарии объектных файлов
out/%.o : src/%.c inc/%.h
	$(CC) $(CFLAGS) $< -o $@

out/main.o : src/main.c
	$(CC) $(CFLAGS) $^ -o $@

# Приложение с ипользованием статической библиотеки
app_with_lib.exe: out/main.o $(LIB_PATH)
	$(CC) -o $@ out/main.o -Llib -ldb $(LDFLAGS)

all: lib app.exe app_with_lib.exe

# Другое
.PHONY : clean lib

clean : 
	rm -rf out/*.o *.exe lib/*.a
