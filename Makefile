# Nombre del ejecutable
EXEC = exam_scheduler

# Archivos fuente
SRCS = Proyecto.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Compilador
CXX = g++

# Opciones de compilación
CXXFLAGS = -Wall -std=c++11

# Objetivo por defecto
all: $(EXEC)

# Compilar el ejecutable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar archivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar el programa
run: $(EXEC)
	./$(EXEC)

# Limpiar archivos generados
clean:
	rm -f $(EXEC) $(OBJS) *.sol *.res *.pen

.PHONY: all run clean

