final: main.o api.o attribute_type.o b_plus_tree.o buffer_manager.o catalog_manager.o index_manager.o interpreter.o record_manager.o
	g++ -O2 -g -pipe -std=c++11 main.o api.o attribute_type.o b_plus_tree.o buffer_manager.o catalog_manager.o index_manager.o interpreter.o record_manager.o -o minisql

main.o: main.cpp 
	g++ -O2 -c -g -pipe main.cpp -std=c++11	
api.o: api.cpp api.h
	g++ -O2 -c -g -pipe api.cpp -std=c++11	
attribute_type.o: attribute_type.cpp attribute_type.h
	g++ -O2 -c -g -pipe attribute_type.cpp -std=c++11	
b_plus_tree.o: b_plus_tree.cpp b_plus_tree.h
	g++ -O2 -c -g -pipe b_plus_tree.cpp -std=c++11	
buffer_manager.o: buffer_manager.cpp buffer_manager.h
	g++ -O2 -c -g -pipe buffer_manager.cpp -std=c++11	
catalog_manager.o: catalog_manager.cpp catalog_manager.h
	g++ -O2 -c -g -pipe catalog_manager.cpp -std=c++11	
index_manager.o: index_manager.cpp index_manager.h
	g++ -O2 -c -g -pipe index_manager.cpp -std=c++11	
interpreter.o: interpreter.cpp interpreter.h
	g++ -O2 -c -g -pipe interpreter.cpp -std=c++11	
record_manager.o: record_manager.cpp record_manager.h
	g++ -O2 -c -g -pipe record_manager.cpp -std=c++11	

clean:
# windows
	del /S *.o
	del /S minisql.exe
# Linux
#   rm -f *.o
# 	rm -f minisql.exe
