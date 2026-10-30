
CC       = c++
CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iincludes -Iexternal -Iexternal/glew/include \
		   -DGLEW_STATIC -DGLEW_NO_GLU \
		   -MMD -MP

LDLIBS = -lglfw -lGL

ROOT_NAME ?= scop
NAME      = $(ROOT_NAME).a
HEADER    = includes/$(ROOT_NAME).h

SRC_DIR   = src
OBJ_DIR   = objs

GLEW_DIR   = external/glew
GLEW_SRC   = $(GLEW_DIR)/src/glew.c
GLEW_FLAGS = -O2 -DGLEW_STATIC -DGLEW_NO_GLU -I$(GLEW_DIR)/include

FILES_SRCS = src/main.cpp src/utils.cpp src/Window.cpp src/Camera.cpp src/Shader.cpp src/Mesh.cpp src/Texture.cpp src/Model.cpp src/mymath/mymath.cpp src/mymath/vec3.cpp src/mymath/mat4.cpp src/Importer.cpp

FILES_OBJS = $(FILES_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
FILES_DEPS = $(FILES_OBJS:.o=.d)

EXECUTABLE = scop

ARGS ?= models/teapot2.obj 1.0

BONUS_EXECUTABLE = scop_bonus
BONUS_OBJ_DIR    = objs_bonus
BONUS_CXXFLAGS   = $(CXXFLAGS) -DSTB_LOADER
BONUS_OBJS       = $(FILES_SRCS:$(SRC_DIR)/%.cpp=$(BONUS_OBJ_DIR)/%.o)
BONUS_DEPS       = $(BONUS_OBJS:.o=.d)

all: $(NAME)
ifdef EXECUTABLE
all: $(EXECUTABLE)
endif

$(NAME): $(FILES_OBJS) $(OBJ_DIR)/glew.o
	ar rcs $(NAME) $(FILES_OBJS) $(OBJ_DIR)/glew.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BONUS_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(BONUS_CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/glew.o: $(GLEW_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(GLEW_FLAGS) -c $< -o $@

$(BONUS_OBJ_DIR)/glew.o: $(GLEW_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(GLEW_FLAGS) -c $< -o $@

$(BONUS_EXECUTABLE): $(BONUS_OBJS) $(BONUS_OBJ_DIR)/glew.o
	$(CXX) -o $(BONUS_EXECUTABLE) $(BONUS_OBJS) $(BONUS_OBJ_DIR)/glew.o $(LDLIBS)

bonus: $(BONUS_EXECUTABLE)

ifdef EXECUTABLE
$(EXECUTABLE): $(FILES_OBJS) $(OBJ_DIR)/glew.o
	$(CXX) -o $(EXECUTABLE) $(FILES_OBJS) $(OBJ_DIR)/glew.o $(LDLIBS)
endif

VALGRIND_FLAGS = --leak-check=full \
		--show-leak-kinds=definite \
		--errors-for-leak-kinds=definite \
		--error-exitcode=1 \
		--suppressions=valgrind.supp

run_check: $(EXECUTABLE)
	@valgrind $(VALGRIND_FLAGS) ./scop $(ARGS); \
	if [ $$? -eq 0 ]; then \
		echo "\n\n \033[32m No leaks found in your code \033[0m"; \
	else \
		echo "\n\n \033[31m Leaks found in your code \033[0m"; \
	fi

run_check_bonus: $(BONUS_EXECUTABLE)
	@valgrind $(VALGRIND_FLAGS) ./$(BONUS_EXECUTABLE) $(ARGS); \
	if [ $$? -eq 0 ]; then \
		echo "\n\n \033[32m No leaks found in your code (bonus) \033[0m"; \
	else \
		echo "\n\n \033[31m Leaks found in your code (bonus) \033[0m"; \
	fi

clean:
	rm -f $(FILES_OBJS) $(FILES_DEPS) $(BONUS_OBJS) $(BONUS_DEPS) $(OBJ_DIR)/glew.o $(BONUS_OBJ_DIR)/glew.o

fclean: clean
	rm -f $(NAME) $(EXECUTABLE) $(BONUS_EXECUTABLE)
	rm -rf $(OBJ_DIR) $(BONUS_OBJ_DIR)

re: fclean all

.PHONY: all bonus clean fclean re run_check run_check_bonus

-include $(FILES_DEPS)
-include $(BONUS_DEPS)
