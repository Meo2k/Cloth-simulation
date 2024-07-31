# Đặt tên cho file thực thi
TARGET = test

# Đặt đường dẫn
SRCDIR = ../src
INCDIR = $(SRCDIR)/include
LIBDIR = $(SRCDIR)/lib
TESTDIR = .
# Đường dẫn chứa file .rc và .res

# Các file mã nguồn và đối tượng
SOURCES = $(TESTDIR)/test.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# File resource
RESOURCE = $(TESTDIR)/icon.rc
RESOURCE_OBJ = $(TESTDIR)/icon.res

# Lựa chọn trình biên dịch
CXX = g++
CXXFLAGS = -I$(INCDIR)
LDFLAGS = -L$(LIBDIR) -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lglu32 -lsfml-audio

# Mục tiêu mặc định
all: $(TARGET)

# Quy tắc để tạo file thực thi
$(TARGET): $(OBJECTS) $(RESOURCE_OBJ)
	$(CXX) -mwindows $(OBJECTS) $(RESOURCE_OBJ) -o $@ $(LDFLAGS)

# Quy tắc để biên dịch các file mã nguồn thành file đối tượng
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Quy tắc để biên dịch file resource (.rc) thành file resource (.res)
$(RESOURCE_OBJ): $(RESOURCE)
	windres $(RESOURCE) -O coff -o $(RESOURCE_OBJ)

# Quy tắc dọn dẹp
clean:
	rm -f $(OBJECTS) $(TARGET) $(RESOURCE_OBJ)

# Mục tiêu để biên dịch và chạy chương trình
run: $(TARGET)
	./$(TARGET)
