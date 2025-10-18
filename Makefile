TARGET = australair
SRC_DIR = src
LIB_DIR = incl

# --- Archivos Fuente ---
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/MainWindow.cpp \
          $(SRC_DIR)/Database.cpp \
		  $(SRC_DIR)/AustralairBudget.cpp \
          $(SRC_DIR)/Buttons.cpp

HEADERS = $(LIB_DIR)/MainWindow.hpp \
          $(LIB_DIR)/Database.hpp \
          $(LIB_DIR)/AustralairBudget.hpp

RESOURCES = $(SRC_DIR)/resources.qrc

# --- Variables de Qt y Herramientas ---
QT_VER ?= 6
QMAKE = qmake$(QT_VER)
RCC = $(shell $(QMAKE) -query QT_INSTALL_LIBEXECS)/rcc
MOC = /usr/lib/qt6/libexec/moc

# --- Rutas de instalación de Qt ---
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
    -I$(QT_INCLUDE_PATH)/QtPrintSupport \
    -I$(LIB_DIR)

LDFLAGS = -L$(QT_LIB_PATH)

LIBS = \
    -lQt$(QT_VER)Core \
    -lQt$(QT_VER)Gui \
    -lQt$(QT_VER)Widgets \
    -lQt$(QT_VER)Sql \
    -lQt$(QT_VER)DBus \
    -lQt$(QT_VER)PrintSupport

# --- Archivos generados ---
OBJECTS_SOURCES = $(SOURCES:.cpp=.o)
MOC_SOURCES = $(LIB_DIR)/MainWindow_moc.cpp \
              $(LIB_DIR)/AustralairBudget_moc.cpp
OBJECTS_MOC = $(MOC_SOURCES:.cpp=.o)
RCC_SOURCES = $(RESOURCES:.qrc=_qrc.cpp)
OBJECTS_RCC = $(RCC_SOURCES:.cpp=.o)

OBJECTS = $(OBJECTS_SOURCES) $(OBJECTS_MOC) $(OBJECTS_RCC)

# --- Reglas principales ---

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "🔧 Enlazando $(TARGET)..."
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "✅ Compilación completa."

%.o: %.cpp
	@echo "🧩 Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regla MOC ---

$(LIB_DIR)/MainWindow_moc.cpp: $(LIB_DIR)/MainWindow.hpp
	@echo "⚙️ Generando MOC para $<..."
	$(MOC) $< -o $@

$(LIB_DIR)/AustralairBudget_moc.cpp: $(LIB_DIR)/AustralairBudget.hpp
	@echo "⚙️ Generando MOC para $<..."
	$(MOC) $< -o $@

%_qrc.cpp: %.qrc
	@echo "📦 Procesando recursos $<..."
	$(RCC) $< -o $@

# --- Utilidades ---

clean:
	@echo "🧹 Limpiando archivos intermedios..."
	rm -f $(OBJECTS_SOURCES) $(OBJECTS_MOC) $(OBJECTS_RCC) $(MOC_SOURCES) $(RCC_SOURCES) $(TARGET)
	@echo "Limpieza completa."

run: $(TARGET)
	@echo "🚀 Ejecutando aplicación..."
	./$(TARGET)
re: clean all run

.PHONY: all clean run re
