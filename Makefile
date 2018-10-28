all:
	make skylake

skylake:
	gcc find_imc.c -o find_imc -DSKYLAKE

broadwell:
	gcc find_imc.c -o find_imc -DBROADWELL

haswell:
	gcc find_imc.c -o find_imc -DHASWELL

ivybridge:
	gcc find_imc.c -o find_imc -DIVYBRIDGE

clean:
	rm -rf find_imc *~
