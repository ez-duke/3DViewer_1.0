CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g
C_FILES = backend/*.c
H_FILES = backend/*.h tests/*.h
TEST_FILES = tests/*.c
BUILD_PATH = frontend/3d_viewer/build/Manual_Build

ifeq ($(shell uname), Linux)
	CHECKFLAGS=-lcheck -lm -lpthread -lrt -lsubunit
	LAUNCHFLAGS=LIBGL_ALWAYS_SOFTWARE=1 
else
	CHECKFLAGS=-lcheck -lm -lpthread
	EXTENDED_PATH=/3d_viewer.app/Contents/MacOS
endif

all: launch

install:
#	sudo apt install qtbase5-dev qt5-qmake
	qmake -o $(BUILD_PATH)/Makefile frontend/3d_viewer/3d_viewer.pro
	cd $(BUILD_PATH) && make

launch: install
ifeq ($(shell uname), Linux)
	chmod 0700 /run/user/1000/
endif
	cd $(BUILD_PATH) && $(LAUNCHFLAGS).$(EXTENDED_PATH)/3d_viewer

uninstall: clean
	rm -rf $(BUILD_PATH)

dvi:
	doxygen Doxyfile
ifeq ($(shell uname), Darwin)
	open ./doxygen/html/index.html
endif

dist:
	cd .. && tar -czf 3DViewer.tar.gz src

tests: clean $(TEST_FILES) ${C_FILES}
	@$(CC) $(CFLAGS) $(TEST_FILES) ${C_FILES} $(CHECKFLAGS) -o test_lib
	@./test_lib

gcov_report: clean $(TEST_FILES) ${C_FILES}
	@$(CC) $(FLAGS) --coverage $(TEST_FILES) ${C_FILES} -o gcov_report $(CHECKFLAGS)
	@./gcov_report
	@lcov -t "./gcov_report" -o gcov_report.info --no-external -c -d .
	@genhtml -o rpn_report gcov_report.info
ifeq ($(shell uname), Darwin)
	open ./rpn_report/index.html
endif

style: clean
	cppcheck --enable=all --suppress=missingIncludeSystem $(C_FILES) $(H_FILES) $(TEST_FILES)
	clang-format -style=Google -n $(C_FILES) $(H_FILES) $(TEST_FILES)

clang-format:
	clang-format -style=Google -i $(C_FILES) $(H_FILES) $(TEST_FILES)

valgrind: tests
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file=valgrind_report.txt ./test_lib

clean:
	rm -rf *.o *.a report *.gcno *.gcda *.info *.txt gcov_report test_lib rpn_report valgrind_report.txt doxygen
