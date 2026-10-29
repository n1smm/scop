
CXX      = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iincludes -Iexternal \
		   -MMD -MP

LDLIBS = -lglfw -lGLEW -lGL  \

ROOT_NAME ?= scop
NAME      = $(ROOT_NAME).a
HEADER    = includes/$(ROOT_NAME).h

SRC_DIR   = src
OBJ_DIR   = objs

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

$(NAME): $(FILES_OBJS)
	ar rcs $(NAME) $(FILES_OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BONUS_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CXX) $(BONUS_CXXFLAGS) -c $< -o $@

$(BONUS_EXECUTABLE): $(BONUS_OBJS)
	$(CXX) -o $(BONUS_EXECUTABLE) $(BONUS_OBJS) $(LDLIBS)

bonus: $(BONUS_EXECUTABLE)

ifdef EXECUTABLE
$(EXECUTABLE): $(FILES_OBJS)
	$(CXX) -o $(EXECUTABLE) $(FILES_OBJS) $(LDLIBS)
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
	rm -f $(FILES_OBJS) $(FILES_DEPS) $(BONUS_OBJS) $(BONUS_DEPS)

fclean: clean
	rm -f $(NAME) $(EXECUTABLE) $(BONUS_EXECUTABLE)
	rm -rf $(OBJ_DIR) $(BONUS_OBJ_DIR)

re: fclean all

.PHONY: all bonus clean fclean re run_check run_check_bonus

-include $(FILES_DEPS)
-include $(BONUS_DEPS)
