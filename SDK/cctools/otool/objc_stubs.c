#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "mach-o/loader.h"
#include "stuff/allocate.h"
#include "stuff/bytesex.h"
#include "stuff/symbol.h"
#include "dyld_bind_info.h"
#include "stuff/bool.h"

struct objc_class;
typedef struct objc_class *Class;

Class objc_getClass(const char *name) {
	(void)name;
	return 0;
}

Class objc_getMetaClass(const char *name) {
	(void)name;
	return 0;
}

void print_objc2_32bit(
	int cputype,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct nlist *symbols,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	struct symbol *sorted_symbols,
	unsigned int nsorted_symbols,
	struct relocation_info *ext_relocs,
	unsigned int next_relocs,
	struct relocation_info *loc_relocs,
	unsigned int nloc_relocs,
	enum bool verbose)
{
	(void)cputype;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)symbols;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)sorted_symbols;
	(void)nsorted_symbols;
	(void)ext_relocs;
	(void)next_relocs;
	(void)loc_relocs;
	(void)nloc_relocs;
	(void)verbose;
}

void print_objc2_64bit(
	int cputype,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct nlist_64 *symbols64,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	struct symbol *sorted_symbols,
	unsigned int nsorted_symbols,
	struct relocation_info *ext_relocs,
	unsigned int next_relocs,
	struct relocation_info *loc_relocs,
	unsigned int nloc_relocs,
	struct dyld_bind_info *dbi,
	uint64_t ndbi,
	enum bool verbose,
	enum bool Vflag)
{
	(void)cputype;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)symbols64;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)sorted_symbols;
	(void)nsorted_symbols;
	(void)ext_relocs;
	(void)next_relocs;
	(void)loc_relocs;
	(void)nloc_relocs;
	(void)dbi;
	(void)ndbi;
	(void)verbose;
	(void)Vflag;
}

enum bool print_objc_segment(
	int mh_cputype,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct symbol *sorted_symbols,
	unsigned int nsorted_symbols,
	enum bool verbose)
{
	(void)mh_cputype;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)sorted_symbols;
	(void)nsorted_symbols;
	(void)verbose;
	return FALSE;
}

void print_objc_protocol_section(
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	enum bool verbose)
{
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)verbose;
}

void print_objc_string_object_section(
	char *sectname,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	enum bool verbose)
{
	(void)sectname;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)verbose;
}

void print_objc_string_object_section_64(
	char *sectname,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	int cputype,
	struct nlist_64 *symbols64,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	struct symbol *sorted_symbols,
	unsigned int nsorted_symbols,
	enum bool verbose)
{
	(void)sectname;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)cputype;
	(void)symbols64;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)sorted_symbols;
	(void)nsorted_symbols;
	(void)verbose;
}

void print_objc_runtime_setup_section(
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	enum bool verbose)
{
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)verbose;
}

char *get_objc2_64bit_cfstring_name(
	uint64_t p,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct nlist_64 *symbols64,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	int cputype)
{
	(void)p;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)symbols64;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)cputype;
	return NULL;
}

char *get_objc2_64bit_class_name(
	uint64_t p,
	uint64_t address_of_p,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct nlist_64 *symbols64,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	int cputype)
{
	(void)p;
	(void)address_of_p;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)symbols64;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)cputype;
	return NULL;
}

uint64_t get_objc2_64bit_selref(
	uint64_t address_of_p,
	struct load_command *load_commands,
	unsigned int ncmds,
	unsigned int sizeofcmds,
	int object_byte_sex,
	char *object_addr,
	unsigned int object_size,
	struct nlist_64 *symbols64,
	unsigned int nsymbols,
	char *strings,
	unsigned int strings_size,
	int cputype)
{
	(void)address_of_p;
	(void)load_commands;
	(void)ncmds;
	(void)sizeofcmds;
	(void)object_byte_sex;
	(void)object_addr;
	(void)object_size;
	(void)symbols64;
	(void)nsymbols;
	(void)strings;
	(void)strings_size;
	(void)cputype;
	return 0;
}