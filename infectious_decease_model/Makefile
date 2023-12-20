COMPILER	=	gcc

FLAGS		=	-O0 -fopenmp

LIBRARIES	=	-lm 

OBJECTS		=	society_model.o functions.o parameters.o iterations.o initial_conditions.o

AUX = functions.o

main:	main.c $(OBJECTS)
	$(COMPILER) $(FLAGS) -o main.bin main.c  $(OBJECTS) $(LIBRARIES)

functions.o: functions.c functions.h
	$(COMPILER) $(FLAGS) -c functions.c $(LIBRARIES)

society_model.o: society_model.c society_model.h $(AUX)
	$(COMPILER) $(FLAGS) -c society_model.c $(AUX) $(LIBRARIES)
	
parameters.o: parameters.c parameters.h $(AUX)
	$(COMPILER) $(FLAGS) -c parameters.c $(AUX) $(LIBRARIES)
	
iterations.o: iterations.c iterations.h $(AUX)
	$(COMPILER) $(FLAGS) -c iterations.c $(AUX) $(LIBRARIES)
	
initial_conditions.o: initial_conditions.c initial_conditions.h $(AUX)
	$(COMPILER) $(FLAGS) -c initial_conditions.c $(AUX) $(LIBRARIES)

clean:
	rm -rf *.o libACO.a *.dSYM rm -f HelloWorld stdint foverflow Pointer Array1D Array3D ParallelRecursion1D ParallelRecursion1Dimproved Structure StructureArray FunctionPointer StructurePointer indexx qsort IO valgrindtest read_mtree galaxyGC