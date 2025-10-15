TARGET = australair
SRC_DIR = src
LIB_DIR = incl

# --- Archivos Fuente ---
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/MainWindow.cpp \
          $(SRC_DIR)/Database.cpp

HEADERS = $(LIB_DIR)/MainWindow.hpp \
          $(LIB_DIR)/Database.hpp \
          $(LIB_DIR)/AustralairBudget.hpp

RESOURCES = $(SRC_DIR)/resources.qrc

# --- Variables de Qt y Herramientas ---
QT_VER ?= 6
QMAKE = qmake$(QT_VER)
RCC = $(shell $(QMAKE) -query QT_INSTALL_LIBEXECS)/rcc
#MOC = $(shell which moc$(QT_VER) 2>/dev/null || which moc 2>/dev/null)
MOC = /usr/lib/qt6/libexec/moc

# Fallback para rutas de Qt (útil si qmake -query falla en un entorno específico)
QT_INCLUDE_PATH = $(shell $(QMAKE) -query QT_INSTALL_HEADERS 2>/dev/null || echo /usr/include/x86_64-linux-gnu/qt6)
QT_LIB_PATH = $(shell $(QMAKE) -query QT_INSTALL_LIBS 2>/dev/null || echo /usr/lib/x86_64-linux-gnu)

# --- Variables de Compilación ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fPIC \
    -I$(QT_INCLUDE_PATH) \
    -I$(QT_INCLUDE_PATH)/QtCore \
    -I$(QT_INCLUDE_PATH)/QtGui \
    -I$(QT_INCLUDE_PATH)/QtWidgets \
    -I$(QT_INCLUDE_PATH)/QtSql \
    -I$(LIB_DIR)

LDFLAGS = -L$(QT_LIB_PATH)
# Nota: La librería QtSql está correctamente incluida aquí.
LIBS = -lQt$(QT_VER)Core -lQt$(QT_VER)Widgets -lQt$(QT_VER)Sql -lQt$(QT_VER)Gui -lQt$(QT_VER)DBus

# --- Archivos Generados e Intermedios ---
OBJECTS_SOURCES = $(SOURCES:.cpp=.o)
MOC_SOURCES = $(LIB_DIR)/MainWindow_moc.cpp
OBJECTS_MOC = $(MOC_SOURCES:.cpp=.o)
RCC_SOURCES = $(RESOURCES:.qrc=_qrc.cpp)
OBJECTS_RCC = $(RCC_SOURCES:.cpp=.o)

# Lista FINAL de todos los objetos para enlazar
OBJECTS = $(OBJECTS_SOURCES) $(OBJECTS_MOC) $(OBJECTS_RCC)

# ====================================================================
# --- Reglas Principales ---
# ====================================================================

all: $(TARGET)

# Regla de Enlace Final
$(TARGET): $(OBJECTS)
	@echo "🔧 Enlazando $(TARGET)..."
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "✅ Compilación completa."

# Regla para Compilar Archivos .cpp (incluyendo los de MOC y RCC)
%.o: %.cpp
	@echo "🧩 Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para Generar MOC (Meta-Object Compiler)
$(LIB_DIR)/MainWindow_moc.cpp: $(LIB_DIR)/MainWindow.hpp
	@echo "⚙️ Generando MOC para $<..."
	$(MOC) $< -o $@

# Regla para Procesar Recursos (Resource Compiler)
%_qrc.cpp: %.qrc
	@echo "📦 Procesando recursos $<..."
	$(RCC) $< -o $@

# ====================================================================
# --- Reglas de Utilidad ---
# ====================================================================

clean:
	@echo "🧹 Limpiando archivos intermedios..."
	rm -f $(OBJECTS_SOURCES) $(OBJECTS_MOC) $(OBJECTS_RCC) $(MOC_SOURCES) $(RCC_SOURCES) $(TARGET)
	@echo "Limpieza completa."

run: $(TARGET)
	@echo "🚀 Ejecutando aplicación..."
	./$(TARGET)

.PHONY: all clean run