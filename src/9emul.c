#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>

/* space between top of stack/bottom of heap */
#define THUNDERDOME (1024 * 1024 * 128)

/* fixed stack size */
#define STACKSZ 1024 * 1024 * 16

struct Exec {
	uint32_t magic; /*  magic number */
	uint32_t text;  /*  size of text segment */
	uint32_t data;  /*  size of initialized data */
	uint32_t bss;   /*  size of uninitialized data */
	uint32_t syms;  /*  size of symbol table */
	uint32_t entry; /*  entry point */
	uint32_t pcsz;  /*  size of pc/line number table */
};
typedef struct Exec Exec;

struct symbol {
	uint32_t val;
	char	 type;
	char*	 name;
};
typedef struct symbol symbol;

struct vaddr_space {
	void  *vspace;
	uint64_t data;
	uint64_t heap;
	uint64_t stack;
};
typedef struct vaddr_space vaddr_space;

int alloc_vaddrspace(vaddr_space *prog, Exec *header) {
	int psize;
	int text_pad;
	
	psize = getpagesize();
	text_pad = header->text % psize;

	prog->vspace = calloc(1, header->text + text_pad + header->data + header->bss +
							 header->syms + header->pcsz + THUNDERDOME + STACKSZ);

	prog->data  = header->text + text_pad;
	prog->heap  = prog->data + header->data + header->bss + header->bss + header->pcsz;
	prog->stack = prog->heap + THUNDERDOME;

	if(!prog->vspace)
		return 1;
	return 0;
}

int main(int argc, char *argv[]) {
	FILE*	    in;
	Exec	    header;
	vaddr_space prog;
	int ret;

	if(argc != 2 || !(in = fopen(argv[1], "r"))) {
		printf("usage: %s <a.out executable>\n", argv[0]);
		return 1;
	}

	fread(&header, 4, 7, in);

	/* XXX */
	if(alloc_vaddrspace(&prog, &header))
		ret = 1;

	fclose(in);
	free(prog.vspace);

	ret = 0;
	return ret;
}
